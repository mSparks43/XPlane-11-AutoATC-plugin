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

void registerDatarefs(){
       /*XPLMDataRef gCounterDataRef =XPLMRegisterDataAccessor("traf/mw_def", xplmType_Float, 1, NULL,
NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL);
    gCounterDataRef=XPLMFindDataRef ("traf/mw_def");
    XPLMSetDataf(gCounterDataRef, 0);*/


XPLMRegisterDataAccessor("traf/nw_def", xplmType_Float, 1, NULL,
NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL);
XPLMRegisterDataAccessor("traf/lgear", xplmType_Float, 1, NULL,
NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL);
XPLMRegisterDataAccessor("traf/flaps", xplmType_Float, 1, NULL,
NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL);
/*XPLMRegisterDataAccessor("traf/lgear", xplmType_Float, 1, NULL,
NULL, getGearState, setGearState, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL);*/
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
/*XPLMFindDataRef ("traf/mw_def");
XPLMFindDataRef ("traf/nw_def");
XPLMFindDataRef ("traf/lgear");
XPLMFindDataRef ("traf/flaps");
//XPLMFindDataRef ("traf/lgear");
XPLMFindDataRef ("traf/nw_rot");
XPLMFindDataRef ("traf/bcn");
XPLMFindDataRef ("traf/strobe");
XPLMFindDataRef ("traf/land");
XPLMFindDataRef ("traf/nav");
XPLMFindDataRef ("traf/taxi");
XPLMFindDataRef ("traf/sb");
XPLMFindDataRef ("traf/vint");
XPLMFindDataRef ("traf/touch_down");
XPLMFindDataRef ("traf/vint_circle");*/
}