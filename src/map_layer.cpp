#include "XPLMMap.h"
#include "XPLMGraphics.h"
#include <string.h>
#include <stdio.h>
#include "tcas.h"

#include <math.h> 
#if IBM
	#include <windows.h>
#endif
#if LIN
	#include <GL/gl.h>
#elif __GNUC__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

XPLMMapLayerID		g_layer = NULL;

static int	coord_in_rect(float x, float y, const float bounds_ltrb[4])  {	
    return ((x >= bounds_ltrb[0]) && (x < bounds_ltrb[2]) && (y >= bounds_ltrb[3]) && (y < bounds_ltrb[1])); }

void createOurMapLayer(const char * mapIdentifier, void * refcon);

static void prep_cache(         XPLMMapLayerID layer, const float * inTotalMapBoundsLeftTopRightBottom, XPLMMapProjectionID projection, void * inRefcon);
//static void draw_markings(      XPLMMapLayerID layer, const float * inMapBoundsLeftTopRightBottom, float zoomRatio, float mapUnitsPerUserInterfaceUnit, XPLMMapStyle mapStyle, XPLMMapProjectionID projection, void * inRefcon);
static void draw_marking_icons( XPLMMapLayerID layer, const float * inMapBoundsLeftTopRightBottom, float zoomRatio, float mapUnitsPerUserInterfaceUnit, XPLMMapStyle mapStyle, XPLMMapProjectionID projection, void * inRefcon);
static void draw_marking_labels(XPLMMapLayerID layer, const float * inMapBoundsLeftTopRightBottom, float zoomRatio, float mapUnitsPerUserInterfaceUnit, XPLMMapStyle mapStyle, XPLMMapProjectionID projection, void * inRefcon);
static void will_be_deleted(    XPLMMapLayerID layer, void * inRefcon);


void MapLayerStop(void)
{
	// Clean up our map layer: if we created it, we should be good citizens and destroy it before the plugin is unloaded
	if(g_layer)
	{
		// Triggers the will-be-deleted callback of the layer, causing g_layer to get set back to NULL
		XPLMDestroyMapLayer(g_layer);
	}
}

int MapLayerEnable(void)
{
	// We want to create our layer in the standard map used in the UI (not other maps like the IOS).
	// If the map already exists in X-Plane (i.e., if the user has opened it), we can create our layer immediately.
	// Otherwise, though, we need to wait for the map to be created, and only *then* can we create our layers.
	if(XPLMMapExists(XPLM_MAP_USER_INTERFACE))
	{
		createOurMapLayer(XPLM_MAP_USER_INTERFACE, NULL);
	}
	// Listen for any new map objects that get created
	XPLMRegisterMapCreationHook(&createOurMapLayer, NULL);
	return 1;
}
//PLUGIN_API void XPluginDisable(void) { }
//PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam) { }

void createOurMapLayer(const char * mapIdentifier, void * refcon)
{
	if(!g_layer && // Confirm we haven't created our markings layer yet (e.g., as a result of a previous callback), or if we did, it's been destroyed
			!strcmp(mapIdentifier, XPLM_MAP_USER_INTERFACE)) // we only want to create a layer in the normal user interface map (not the IOS)
	{
		XPLMCreateMapLayer_t params;
		params.structSize = sizeof(XPLMCreateMapLayer_t);
		params.mapToCreateLayerIn = XPLM_MAP_USER_INTERFACE;
		params.willBeDeletedCallback = &will_be_deleted;
		params.prepCacheCallback = &prep_cache;
		params.showUiToggle = 1;
		params.refcon = NULL;
		params.layerType = xplm_MapLayer_Markings;
		params.drawCallback = NULL;
		params.iconCallback = &draw_marking_icons;
		params.labelCallback = &draw_marking_labels;
		params.layerName = "AutoATC aircraft";
		// Note: this could fail (return NULL) if we hadn't already confirmed that params.mapToCreateLayerIn exists in X-Plane already
		g_layer = XPLMCreateMapLayer(&params);
	}
}

int s_num_cached_coords = 0;
#define MAX_COORDS (360 * 180)
float s_cached_x_coords[MAX_COORDS]; // The map x coordinates at which we will draw our icons; only the range [0, s_num_cached_coords) are valid
float s_cached_y_coords[MAX_COORDS]; // The map y coordinates at which we will draw our icons; only the range [0, s_num_cached_coords) are valid
float s_cached_lon_coords[MAX_COORDS]; // The real latitudes that correspond to our cached map (x, y) coordinates; only the range [0, s_num_cached_coords) are valid
float s_cached_lat_coords[MAX_COORDS]; // The real latitudes that correspond to our cached map (x, y) coordinates; only the range [0, s_num_cached_coords) are valid
//float s_icon_width = 0; // The width, in map units, that we should draw our icons.

void prep_cache(XPLMMapLayerID layer, const float * inTotalMapBoundsLeftTopRightBottom, XPLMMapProjectionID projection, void * inRefcon)
{
	// We're simply going to cache the locations, in *map* coordinates, of all the places we want to draw.
	s_num_cached_coords = 0;
	for(int lon = -180; lon < 180; ++lon)
	{
		for(int lat =  -90; lat <  90; ++lat)
		{
			float x, y;
			const float offset = 0.25; // to avoid drawing on grid lines
			XPLMMapProject(projection, lat + offset, lon + offset, &x, &y);
			if(coord_in_rect(x, y, inTotalMapBoundsLeftTopRightBottom))
			{
				s_cached_x_coords[s_num_cached_coords] = x;
				s_cached_y_coords[s_num_cached_coords] = y;
				s_cached_lon_coords[s_num_cached_coords] = lon + offset;
				s_cached_lat_coords[s_num_cached_coords] = lat + offset;
				++s_num_cached_coords;
			}
		}
	}

	// Because the map uses true cartographical projections, the size of 1 meter in map units can change
	// depending on where you are asking about. We'll ask about the midpoint of the available bounds
	// and assume the answer won't change too terribly much over the size of the maps shown in the UI.
	/*const float midpoint_x = (inTotalMapBoundsLeftTopRightBottom[0] + inTotalMapBoundsLeftTopRightBottom[2]) / 2;
	const float midpoint_y = (inTotalMapBoundsLeftTopRightBottom[1] + inTotalMapBoundsLeftTopRightBottom[3]) / 2;
	// We'll draw our icons to be 5000 meters wide in the map
	s_icon_width = XPLMMapScaleMeter(projection, midpoint_x, midpoint_y) * 350;*/
}


void draw_marking_icons(XPLMMapLayerID layer, const float * inMapBoundsLeftTopRightBottom, float zoomRatio, float mapUnitsPerUserInterfaceUnit, XPLMMapStyle mapStyle, XPLMMapProjectionID projection, void * inRefcon)
{
    TCASAPI* tcasAPI=getTCASAPI();
    const float midpoint_x = (inMapBoundsLeftTopRightBottom[0] + inMapBoundsLeftTopRightBottom[2]) / 2;
	const float midpoint_y = (inMapBoundsLeftTopRightBottom[1] + inMapBoundsLeftTopRightBottom[3]) / 2;
	// We'll draw our icons to be 5000 meters wide in the map

	float  ts_icon_width = XPLMMapScaleMeter(projection, midpoint_x, midpoint_y)* 50;
	if((34-zoomRatio)>15)
        ts_icon_width*=(12*((34-zoomRatio)-9));
    else if((34-zoomRatio)>10)
        ts_icon_width*=(6*((34-zoomRatio)-9));

    #define SAMPLE_IMG "Resources/plugins/AutoATC/objects/aircrafticons.png"
   // printf("zoom %f %f\n",ts_icon_width,(34-zoomRatio));
    
    for(int i = 0; i < TARGETS; i++)
    {
        if(tcasAPI->alive[i]>0){

        float x, y;
        XPLMMapProject(projection, tcasAPI->alat[i], tcasAPI->alon[i], &x, &y);
        XPLMDrawMapIconFromSheet(
						layer, SAMPLE_IMG,
						((int)tcasAPI->icon[i]/3), ((int)tcasAPI->icon[i]%3), // draw the image cell at (s, t) == (0, 0) (i.e., the bottom left cell in the sample image)
						3, 2, // our sample image is two image cell wide, and two image cells tall
						x, y,
						xplm_MapOrientation_Map, // Orient the icon relative to the map itself, rather than relative to the UI
						tcasAPI->psi[i], // Zero degrees rotation
						ts_icon_width);
        }
    }
	/*for(int coord = 0; coord < s_num_cached_coords; ++coord)
	{
		const float x = s_cached_x_coords[coord];
		const float y = s_cached_y_coords[coord];
		if(coord_in_rect(x, y, inMapBoundsLeftTopRightBottom))
		{
			#define SAMPLE_IMG "Resources/plugins/map-sample-image.png"
			if(coord % 2)
			{
				XPLMDrawMapIconFromSheet(
						layer, SAMPLE_IMG,
						0, 0, // draw the image cell at (s, t) == (0, 0) (i.e., the bottom left cell in the sample image)
						2, 2, // our sample image is two image cell wide, and two image cells tall
						x, y,
						xplm_MapOrientation_Map, // Orient the icon relative to the map itself, rather than relative to the UI
						0, // Zero degrees rotation
						s_icon_width);
			}
			else
			{
				// Draw the image at cell (s, t) == (1, 1) (i.e., the top right cell in the sample image)
				XPLMDrawMapIconFromSheet(layer, SAMPLE_IMG, 1, 1, 2, 2, x, y, xplm_MapOrientation_Map, 0, s_icon_width);
			}
		}
	}*/
}

void draw_marking_labels(XPLMMapLayerID layer, const float * inMapBoundsLeftTopRightBottom, float zoomRatio, float mapUnitsPerUserInterfaceUnit, XPLMMapStyle mapStyle, XPLMMapProjectionID projection, void * inRefcon)
{

	if(zoomRatio >= 18) // don't label when zoomed too far out... everything will run together in a big, illegible mess
	{
        TCASAPI* tcasAPI=getTCASAPI();
        const float midpoint_x = (inMapBoundsLeftTopRightBottom[0] + inMapBoundsLeftTopRightBottom[2]) / 2;
	    const float midpoint_y = (inMapBoundsLeftTopRightBottom[1] + inMapBoundsLeftTopRightBottom[3]) / 2;
        float  ts_icon_width = XPLMMapScaleMeter(projection, midpoint_x, midpoint_y)* 50;
		if((34-zoomRatio)>15)
        	ts_icon_width*=(12*((34-zoomRatio)-9));
    	else if((34-zoomRatio)>10)
        	ts_icon_width*=(6*((34-zoomRatio)-9));

        for(int i = 0; i < TARGETS; i++)
        {
            if(tcasAPI->alive[i]>0){
                float x, y;
                char scratch_buffer[150];
				sprintf(scratch_buffer, "%0.0fft", tcasAPI->aalt[i]);
                XPLMMapProject(projection, tcasAPI->alat[i], tcasAPI->alon[i], &x, &y);
                const float icon_bottom = y - ts_icon_width / 2;
				const float text_center_y = icon_bottom - (mapUnitsPerUserInterfaceUnit * icon_bottom / 2); // top of the text will touch the bottom of the icon
				XPLMDrawMapLabel(layer, scratch_buffer, x, text_center_y, xplm_MapOrientation_Map, 0);
            } 

        }  
		
	}
}

void will_be_deleted(XPLMMapLayerID layer, void * inRefcon)
{
	if(layer == g_layer)
		g_layer = NULL;
}