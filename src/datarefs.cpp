 /*
*****************************************************************************************
*        COPYRIGHT � 2020 Mark Parker/mSparks


GNU Lesser General Public License v3.0
Permissions of this copyleft license are conditioned on making available complete source code of
licensed works and modifications under the same license or the GNU GPLv3. Copyright and license 
notices must be preserved. Contributors provide an express grant of patent rights. 
However, a larger work using the licensed work through interfaces provided by the licensed work may 
be distributed under different terms and without source code for the larger work.
*****************************************************************************************
*/
#define NOMINMAX
 #include "XPLMDataAccess.h"

 #include <stdlib.h>
 #include <string>
#include <algorithm>
float drefV=1.0;
float getGearState(void * inRefcon) {
    return drefV;
}
void setGearState(void * inRefcon,float outvalue) {
    drefV=outvalue;
    //return 0;
}
#define ARRAY_DIM 25

struct	string_dref {
	std::string					string_data;
};
struct	int_dref {
	int					data;
};
static int g_my_idarray[ARRAY_DIM] = { 0 };
static int g_my_afarray[ARRAY_DIM] = { 0 };
static int g_my_xarray[ARRAY_DIM] = { 0 };
static int g_my_yarray[ARRAY_DIM] = { 0 };
static int g_my_zarray[ARRAY_DIM] = { 0 };
static int g_my_damagearray[ARRAY_DIM] = { 0 };
static string_dref acarsinarray;
static string_dref acarsoutarray;
static int_dref receivedAcars;
static int_dref sendAcars;
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
static int getvb(void * refA, void * out_values, int in_offset, int in_max)
{
    char * destination = (char *) out_values;
	string_dref * ref =(string_dref *)refA;
	if(out_values == NULL)
		return ref->string_data.size();
	if(in_offset >= ref->string_data.size())
		return 0;
	int count = std::min(in_max, (int)(ref->string_data.size()) - in_offset);
	for(int i = 0; i < count; ++i)
		destination[i] = ref->string_data[i + in_offset];
	return count;	
}

static void setvb(void * refA, void * in_values, int in_offset, int in_max)
{
    const char * source = (const char *) in_values;
	int new_length = in_offset + in_max;

	string_dref * ref =(string_dref *)refA;
	ref->string_data.resize(new_length);
	for(int i = 0; i < in_max; ++i)
		ref->string_data[i + in_offset] = source[i];

}
// For nunmbers
static int	geti(void * refA)
{
	int_dref *  ref = (int_dref *) refA;

	return ref->data; 
	//return r->m_number_storage;
}

static void	seti(void * refA, int val)
{
	int_dref *  ref = (int_dref *) refA;
	ref->data=val;
}
void registerDatarefs(){
	XPLMRegisterDataAccessor("traf/nw_def", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/lgear", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/flaps", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/nw_rot", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/bcn", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/strobe", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/land", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/nav", xplmType_Float,0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/taxi", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/vint_circle", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/vint", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/touch_down", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("traf/sb", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	//WT3
	XPLMRegisterDataAccessor("cjs/world_traffic/main_gear_retraction_ratio", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/nose_gear_retraction_ratio", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/beacon_lights_on", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/strobe_lights_on", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/wing_landing_lights_on", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/nav_lights_on", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/taxi_lights_on", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/engine_rotation_angle2", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/engine_rotation_angle1", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/engine/POINT_prop_ang_deg0", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/engine/POINT_prop_ang_deg1", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/engine/POINT_tacrad0", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/engineon", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/destroyed", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/altitude", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/flak", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("autoatc/engine/POINT_tacrad1", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/touch_down", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/engine_rpm1", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	XPLMRegisterDataAccessor("cjs/world_traffic/engine_rpm2", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);

	//CSL from https://github.com/kuroneko/libxplanemp/blob/master/src/XPMPMultiplayerObj8.cpp
	XPLMRegisterDataAccessor("libxplanemp/controls/gear_ratio", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/flap_ratio", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/spoiler_ratio", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/speed_brake_ratio", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/slat_ratio", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/wing_sweep_ratio", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/thrust_ratio", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/yoke_pitch_ratio", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/yoke_heading_ratio", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/yoke_roll_ratio", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);

		XPLMRegisterDataAccessor("libxplanemp/controls/landing_lites_on", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/beacon_lites_on", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/strobe_lites_on", xplmType_Float, 0, NULL,
	NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
		XPLMRegisterDataAccessor("libxplanemp/controls/nav_lites_on", xplmType_Float, 0, NULL,
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

	XPLMRegisterDataAccessor("autoatc/acars/in", xplmType_Data, true,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						getvb, setvb,
						&acarsinarray, &acarsinarray);
	XPLMRegisterDataAccessor("autoatc/acars/out", xplmType_Data, true,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						getvb, setvb,
						&acarsoutarray, &acarsoutarray);
	/*XPLMRegisterDataAccessor("autoatc/acars/send", xplmType_Int, true,
						geti, seti,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						&sendAcars, &sendAcars);*/
	XPLMRegisterDataAccessor("autoatc/acars/received", xplmType_Int, true,
						geti, seti,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						&receivedAcars, &receivedAcars);										
	
}
