 #include "XPLMDataAccess.h"
 #include <stdlib.h>
float drefV=1.0;
float getGearState(void * inRefcon) {
    return drefV;
}
void setGearState(void * inRefcon,float outvalue) {
    drefV=outvalue;
    //return 0;
}
#define ARRAY_DIM 25
static int g_my_idarray[ARRAY_DIM] = { 0 };
static int g_my_afarray[ARRAY_DIM] = { 0 };
static int g_my_xarray[ARRAY_DIM] = { 0 };
static int g_my_yarray[ARRAY_DIM] = { 0 };
static int g_my_zarray[ARRAY_DIM] = { 0 };
static int g_my_damagearray[ARRAY_DIM] = { 0 };
 static int getvi(void * ref, int * out_values, int in_offset, int in_max)
{
    int n, r;
    int *g_my_array;
	if(ref==g_my_idarray) 
        g_my_array=g_my_idarray;
	else if(ref==g_my_afarray) 
        g_my_array=g_my_afarray;
    else if(ref==g_my_xarray) 
        g_my_array=g_my_xarray;
    else if(ref==g_my_yarray) 
    {
        g_my_array=g_my_yarray;
    }
    else if(ref==g_my_zarray) 
    {
        g_my_array=g_my_zarray;
    }
    else 
    {
        g_my_array=g_my_damagearray;
    }    
    if(out_values == NULL)
        return ARRAY_DIM;
    r = ARRAY_DIM - in_offset;
    if(r > in_max) r = in_max;

	// Now copy the actual items from our array to the returned memory.
    for(n = 0; n < r; ++n) out_values[n] = g_my_array[n + in_offset]; return r; 
}

static void setvi(void * ref, int * in_values, int in_offset, int in_max)
{
    int n, r; // Calculate the number of items to copy in. This is the lesser of the number // the caller writes and the end of our array. 
    r = ARRAY_DIM - in_offset; 
    if (r > in_max) r = in_max;
     int *g_my_array;
    if(ref==g_my_idarray) 
        g_my_array=g_my_idarray;
	else if(ref==g_my_afarray) 
        g_my_array=g_my_afarray;
    else if(ref==g_my_xarray) 
        g_my_array=g_my_xarray;
    else if(ref==g_my_yarray) 
    {
        g_my_array=g_my_yarray;
    }
    else if(ref==g_my_zarray) 
    {
        g_my_array=g_my_zarray;
    }
    else 
    {
        g_my_array=g_my_damagearray;
    } 
    // Copy the actual data.
    for(n = 0; n < r; ++n)
        g_my_array[n+ in_offset] = in_values[n];
}
static int getvf(void * ref, float * out_values, int in_offset, int in_max)
{
    int n, r;
    int *g_my_array;
    if(ref==g_my_xarray) 
        g_my_array=g_my_xarray;
    else if(ref==g_my_yarray) 
    {
        g_my_array=g_my_yarray;
    }
    else if(ref==g_my_zarray) 
    {
        g_my_array=g_my_zarray;
    }
    else 
    {
        g_my_array=g_my_damagearray;
    }    
    if(out_values == NULL)
        return ARRAY_DIM;
    r = ARRAY_DIM - in_offset;
    if(r > in_max) r = in_max;

	// Now copy the actual items from our array to the returned memory.
    for(n = 0; n < r; ++n) out_values[n] = g_my_array[n + in_offset]; return r; 
}

static void setvf(void * ref, float * in_values, int in_offset, int in_max)
{
    int n, r; // Calculate the number of items to copy in. This is the lesser of the number // the caller writes and the end of our array. 
    r = ARRAY_DIM - in_offset; 
    if (r > in_max) r = in_max;
     int *g_my_array;
    if(ref==g_my_xarray) 
        g_my_array=g_my_xarray;
    else if(ref==g_my_yarray) 
    {
        g_my_array=g_my_yarray;
    }
    else if(ref==g_my_zarray) 
    {
        g_my_array=g_my_zarray;
    }
    else 
    {
        g_my_array=g_my_damagearray;
    } 
    // Copy the actual data.
    for(n = 0; n < r; ++n)
        g_my_array[n+ in_offset] = in_values[n];
}
void registerDatarefs(){
	XPLMRegisterDataAccessor("traf/nw_def", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/lgear", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/flaps", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/nw_rot", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/bcn", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/strobe", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/land", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/nav", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/taxi", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/vint_circle", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/vint", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/touch_down", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/sb", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	//WT3
	XPLMRegisterDataAccessor("cjs/world_traffic/main_gear_retraction_ratio", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/nose_gear_retraction_ratio", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/beacon_lights_on", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/strobe_lights_on", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/wing_landing_lights_on", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/nav_lights_on", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/taxi_lights_on", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/engine_rotation_angle2", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/engine_rotation_angle1", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/engine/POINT_prop_ang_deg0", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/engine/POINT_prop_ang_deg1", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/engine/POINT_tacrad0", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/engineon", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/destroyed", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/altitude", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/flak", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/engine/POINT_tacrad1", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/touch_down", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/engine_rpm1", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/engine_rpm2", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);

	//CSL from https://github.com/kuroneko/libxplanemp/blob/master/src/XPMPMultiplayerObj8.cpp
	XPLMRegisterDataAccessor("libxplanemp/controls/gear_ratio", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/flap_ratio", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/spoiler_ratio", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/speed_brake_ratio", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/slat_ratio", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/wing_sweep_ratio", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/thrust_ratio", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/yoke_pitch_ratio", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/yoke_heading_ratio", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/yoke_roll_ratio", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);

		XPLMRegisterDataAccessor("libxplanemp/controls/landing_lites_on", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/beacon_lites_on", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/strobe_lites_on", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/nav_lites_on", xplmType_Float, 1, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/aircraft/id", xplmType_IntArray, 1, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    getvi, setvi,getvf, setvf, 
    NULL, NULL,g_my_idarray, g_my_idarray);//create it
	XPLMRegisterDataAccessor("autoatc/aircraft/af", xplmType_IntArray, 1, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    getvi, setvi,getvf, setvf, 
    NULL, NULL,g_my_afarray, g_my_afarray);//create it
	XPLMRegisterDataAccessor("autoatc/aircraft/x", xplmType_FloatArray, 1, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    NULL, NULL,getvf, setvf, 
    NULL, NULL,g_my_xarray, g_my_xarray);//create it
	XPLMRegisterDataAccessor("autoatc/aircraft/y", xplmType_FloatArray, 1, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    NULL, NULL,getvf, setvf, 
    NULL, NULL,g_my_yarray, g_my_yarray);//create it

	XPLMRegisterDataAccessor("autoatc/aircraft/z", xplmType_FloatArray, 1, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    NULL, NULL,getvf, setvf, 
    NULL, NULL,g_my_zarray, g_my_zarray);//create it

	XPLMRegisterDataAccessor("autoatc/aircraft/target_damage", xplmType_FloatArray, 1, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    NULL, NULL,getvf, setvf, 
    NULL, NULL,g_my_damagearray, g_my_damagearray);//create it
	
}
