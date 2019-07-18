#include "XPLMInstance.h"
#include <time.h>
/*struct planeid { 
	int			id;
};*/
class AircraftData
{
public:
    float		x;
	float		y;
	float		z;
	float		the;//pitch
	float		phi;//roll
	float		psi;//heading
	float		gear_deploy;
	bool		engineoff;
	float		throttle[8];
	clock_t		time;
	double      remoteTimestamp;
    AircraftData(void);

};
/*class AircraftFrame
{
	public:
	double yOffset;
	char model[512];
	AircraftFrame(char tmodel[512],double tyOffset);
	AircraftFrame(void);
	char* getModel();
	double getOffset();
};
class AircraftFrames
{
private:
	AircraftFrame* airframes[5];
public:
	
    AircraftFrames(void);
	AircraftFrame* getFrame(int id);
};*/
class Aircraft
{
private:
	/*XPLMDataRef	dr_plane_x;
	XPLMDataRef	dr_plane_y;
	XPLMDataRef	dr_plane_z;
	XPLMDataRef	dr_plane_the;
	XPLMDataRef	dr_plane_phi;
	XPLMDataRef	dr_plane_psi;
	XPLMDataRef	dr_plane_gear_deploy;
	XPLMDataRef	dr_plane_throttle;*/
	XPLMObjectRef* g_object=NULL;
	XPLMInstanceRef g_instance[1]={NULL};
	const char * cls_drefs[17]={ "traf/mw_def","traf/nw_def","traf/lgear","traf/flaps","traf/nw_rot","traf/bcn","traf/strobe"
	,"traf/land","traf/nav","traf/taxi","traf/sb","traf/vint","traf/touch_down","traf/vint_circle","traf/vint_circle2", NULL };
	const char * wt3_drefs[17]={ "traf/mw_def","traf/nw_def","cjs/world_traffic/main_gear_retraction_ratio","cjs/world_traffic/nose_gear_retraction_ratio","traf/nw_rot","cjs/world_traffic/beacon_lights_on","cjs/world_traffic/strobe_lights_on"
	,"cjs/world_traffic/wing_landing_lights_on","cjs/world_traffic/nav_lights_on","cjs/world_traffic/taxi_lights_on","traf/sb","traf/vint","cjs/world_traffic/touch_down","cjs/world_traffic/engine_rpm1","cjs/world_traffic/engine_rpm2", NULL };
	/*const char * xmp_drefs[17]={ "traf/mw_def","traf/nw_def","cjs/world_traffic/main_gear_retraction_ratio","cjs/world_traffic/nose_gear_retraction_ratio","traf/nw_rot","cjs/world_traffic/beacon_lights_on","cjs/world_traffic/strobe_lights_on"
	,"cjs/world_traffic/wing_landing_lights_on","cjs/world_traffic/nav_lights_on","cjs/world_traffic/taxi_lights_on","traf/sb","traf/vint","cjs/world_traffic/touch_down","cjs/world_traffic/engine_rpm1","cjs/world_traffic/engine_rpm2", NULL };
	*/
	
	const char * xmp_drefs[19]={ "traf/mw_def","traf/nw_def","libxplanemp/controls/gear_ratio","libxplanemp/controls/flap_ratio","traf/nw_rot","libxplanemp/controls/beacon_lites_on","libxplanemp/controls/strobe_lites_on"
	,"libxplanemp/controls/landing_lites_on","libxplanemp/controls/nav_lites_on","libxplanemp/controls/strobe_lites_on","traf/sb","traf/vint","cjs/world_traffic/touch_down","cjs/world_traffic/engine_rpm1","cjs/world_traffic/engine_rpm2","cjs/world_traffic/engine_rotation_angle1","cjs/world_traffic/engine_rotation_angle2", NULL };
	int ref_style=0;
	bool wt3;
	XPLMProbeRef ground_probe;
    bool inTouchDown;
	clock_t touchDownTime;
	double startMoveTime;
	double rpm;
	double yOffset;
	
	v velocity;
public:
	int id;
	int soundIndex;
	int airFrameIndex=-1;
	AircraftData data;
	AircraftData lastData;
	AircraftData nextData;
	bool toLoadAirframe;
    //typedef XPLMPlaneDrawState_t* state;
	//Aircraft(int AircraftNo,double yOffsetp);
	//Aircraft(int AircraftNo);
	Aircraft();
	void GetAircraftData(AircraftData userdata);
	void SetAircraftData(void);
	v getSndSrc();
	v getVelocity();
	void setModelObject(XPLMObjectRef inObject);
	//void loadedobject(XPLMObjectRef inObject, void *inRef);
	//void load_cb(const char * real_path, void * ref);
};
// Datarefs for the User Aircraft
/**/

 

void initPlanes();   
void stopPlanes();                                
