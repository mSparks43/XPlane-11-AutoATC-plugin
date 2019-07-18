
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "vec_opps.h"
#include "XPLMPlanes.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include <string>
#include "XPLMDisplay.h"
#include "XPLMScenery.h"
#include "XPLMInstance.h"
#include "XPLMPlugin.h"
 #include "XPLMMenus.h"

 #include "aiplane.h"
 #include "jvm.h"

  #include "AISound.h"
 using namespace std;

#define MSG_ADD_DATAREF 0x01000000           //  Add dataref to DRE message
#define PI 3.14159265
static XPLMDataRef		gPlaneX = NULL;
static XPLMDataRef		gPlaneY = NULL;
static XPLMDataRef		gPlaneZ = NULL;
static XPLMDataRef		gPlaneTheta = NULL;
static XPLMDataRef		gPlanePhi = NULL;
static XPLMDataRef		gPlanePsi = NULL;
static XPLMDataRef		gdaylight = NULL;
static XPLMDataRef		gOverRidePlanePosition = NULL;
static XPLMDataRef		gAGL = NULL;
                                  
float	BeginAI();
/*static float	MyFlightLoopCallback0(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon);  */  

// Used to update each aircraft every frame.

static float	MyFlightLoopCallback(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon);   
AircraftData::AircraftData(){
	time=clock();
 }

Aircraft::Aircraft()
{
	ground_probe = XPLMCreateProbe(xplm_ProbeY);
	yOffset=5.0;//yOffsetp;
	airFrameIndex=-1;
	toLoadAirframe=false;
}
  char gBob_debstr3[128];
 
Aircraft aircraft[30];
AircraftSounds soundSystem(aircraft);
//planeid *plnid=NULL;
bool inLoading;
static void loadedobject(XPLMObjectRef inObject, void *inRef){
	int id=*(int *)inRef;
	char debugStr[512];
	sprintf(debugStr,"loaded object %d %d\n",id,inObject);
	XPLMDebugString(debugStr);
	aircraft[id-1].setModelObject(inObject);
	inLoading=false;
	
}
int pid=25;
char found_path[1024];
 static void load_cb(const char * real_path, void * ref)
{
	XPLMObjectRef * dest = (XPLMObjectRef *) ref;
	if(*dest == NULL)
	{
		sprintf(found_path,"%s",real_path);
		printf("queued loading object %s\n",found_path);
	}
	else
		printf("skipped loading object %s\n",real_path);
}
void Aircraft::setModelObject(XPLMObjectRef inObject){
	//XPLMCreateInstance(inObject, drefs);
	if(inObject)
	        {
				if(g_instance[0])
					XPLMDestroyInstance(g_instance[0]);
				if(ref_style==0)
					g_instance[0] = XPLMCreateInstance(inObject, cls_drefs);
				else if(ref_style==1)
					g_instance[0] = XPLMCreateInstance(inObject, wt3_drefs);
				else if(ref_style==2)
					g_instance[0] = XPLMCreateInstance(inObject, xmp_drefs);
            }
}
void Aircraft::GetAircraftData(AircraftData userdata)
{
   
    PlaneData thisData;
	
	
	clock_t timeNow = clock();
	

    

	JVM* jvmO;
	try{
		jvmO=getJVM();
		thisData=jvmO->getPlaneData(id);
	}catch(...){
		XPLMDebugString("Exception getting plane data");
		return;
	}
	
	if(!thisData.live){
		lastData.time=timeNow;
		nextData.time=timeNow;
		
		        if(g_instance[0])
		        {
					printf("dropping object for id= %d\n",id);
					XPLMDestroyInstance(g_instance[0]);
					g_instance[0]=NULL;
					airFrameIndex=-1;
				}
		
		return;
	}
	
	if(!inLoading&&toLoadAirframe){
		toLoadAirframe=false;
		char* af=jvmO->getModel(airFrameIndex);
		char debugStr[512];
		XPLMDebugString("toLoadAirframe\n");
		sprintf(debugStr,"searching for id=%d afI=%d to %s sound=%d\n",id,airFrameIndex,af,soundIndex);
		XPLMDebugString(debugStr);
		int *pid=&id;
		inLoading=true;
		XPLMLoadObjectAsync(af, loadedobject, pid);
		
	}

    if(lastData.time==nextData.time){
        lastData.remoteTimestamp=thisData.timeStamp;
        lastData.time=timeNow;
        lastData.x=thisData.x;
		lastData.y=thisData.y;
		lastData.z=thisData.z;
        lastData.the=thisData.the;
		lastData.phi=thisData.phi;
		lastData.psi=thisData.psi;
        return;
    }

    if(lastData.remoteTimestamp==(double)thisData.timeStamp){
        //we dont have new data
        return;
    }
    if(nextData.time<lastData.time){
        nextData.remoteTimestamp=thisData.timeStamp;
        nextData.time=timeNow+CLOCKS_PER_SEC;
        nextData.x=thisData.x;
		nextData.y=thisData.y;
		nextData.z=thisData.z;
        nextData.the=thisData.the;
		nextData.phi=thisData.phi;
		nextData.psi=thisData.psi;

    }
    clock_t lastTime= lastData.time;
	double duration=((float)timeNow-(float)lastTime)/CLOCKS_PER_SEC;
	if(duration<0)
		duration=0;
    double htWindow=(nextData.remoteTimestamp-lastData.remoteTimestamp)/1000;//the time it should take us in seconds
    if(htWindow<1)
		htWindow=1;

		double pComplete=duration/(htWindow);
		float vx=nextData.x-lastData.x;
		float vy=nextData.y-lastData.y;
		float vz=nextData.z-lastData.z;
		velocity=v((nextData.x-lastData.x)/htWindow,(nextData.y-lastData.y)/htWindow,(nextData.z-lastData.z)/htWindow);


		float x=lastData.x+(vx*pComplete);
		float y=lastData.y+(vy*pComplete);
		float z=lastData.z+(vz*pComplete);


		data.x = x;
		data.y = y;
		data.z = z;

        data.the=thisData.the;
        data.phi=thisData.phi;
        data.psi=thisData.psi;
		if(thisData.airframe==2.0)
			data.engineoff=true;
		else
			data.engineoff=false;
    if(nextData.remoteTimestamp<(thisData.timeStamp-300)){
        //we have a new position to get to
        //set last data to where we are now

        lastData.remoteTimestamp= nextData.remoteTimestamp;
        lastData.time=timeNow;
        lastData.x=nextData.x-(vx*(1-pComplete));
		lastData.y=nextData.y-(vy*(1-pComplete));
		lastData.z=nextData.z-(vz*(1-pComplete));
        lastData.the=nextData.the;
		lastData.phi=nextData.phi;
		lastData.psi=nextData.psi;
        //set next data to this new position
        nextData.remoteTimestamp=thisData.timeStamp;
        nextData.time=timeNow+CLOCKS_PER_SEC;
        nextData.x=thisData.x;
		nextData.y=thisData.y;
		nextData.z=thisData.z;
        nextData.the=thisData.the;
		nextData.phi=thisData.phi;
		nextData.psi=thisData.psi;
    }
	XPLMProbeInfo_t outInfo;
	outInfo.structSize = sizeof(outInfo);
	//XPLMProbeResult ground= 
	XPLMProbeTerrainXYZ(ground_probe,data.x,data.y,data.z,&outInfo);
	double requestedAGL=data.y-outInfo.locationY;
	
	double gearDown=0.0;
	if(requestedAGL<400)
		gearDown=1.0;
	else if(requestedAGL<500){
		gearDown=(100.0f-(requestedAGL-400.0f))/100.0f;
	}	
	//for (int Gear=0; Gear<5; Gear++)
	{
		data.gear_deploy = gearDown;//thisData.gearDown;
		
	}
    for (int Throttle=0; Throttle<8; Throttle++)
	{
		data.throttle[Throttle] = 0.0;//thisData.throttle;
		
	}

	
    {
	
		XPLMProbeInfo_t outInfo;
		outInfo.structSize = sizeof(outInfo);
		XPLMProbeResult ground= XPLMProbeTerrainXYZ(ground_probe,data.x,data.y,data.z,&outInfo);
		double requestedAGL=data.y-outInfo.locationY;
		if(requestedAGL<18.0){
			data.y=outInfo.locationY;
			if(inTouchDown){
				touchDownTime=clock()/CLOCKS_PER_SEC;
				soundSystem.land(id-1);
				inTouchDown=false;
			}
		}
		else if(requestedAGL<=198.0){
			inTouchDown=true;
			requestedAGL=((requestedAGL-18.0)*1.1);
			data.y=outInfo.locationY+requestedAGL;
		}

		if( data.the>0){
			double lift=15.0*tan(data.the*PI/180.0);
			data.y=data.y+lift;
		}
		//check model
		if(airFrameIndex==-1&&airFrameIndex!=thisData.airframe){

			soundIndex=jvmO->getSound(thisData.airframe);
			yOffset=jvmO->getOffset(thisData.airframe);

			ref_style=jvmO->getDrefStyle(thisData.airframe);
			sprintf(gBob_debstr3,"AutoATC switch Airframe %d. at %f style=%d\n",thisData.airframe,yOffset,ref_style);
        	XPLMDebugString(gBob_debstr3);
			airFrameIndex=thisData.airframe;
			toLoadAirframe=true;
			
		}
	}
}
float useNavLights=0.0;
v Aircraft::getSndSrc(){
	v retVal(data.x,data.z,data.y);
	return retVal;
}
v Aircraft::getVelocity(){

	//v retVal(data.x,data.z,data.y);
	return velocity;
}
void Aircraft::SetAircraftData(void)
{

    XPLMDrawInfo_t		dr;
	dr.structSize = sizeof(dr);
	dr.x = data.x;
	dr.y = data.y+yOffset;
	dr.z = data.z;
	dr.pitch = data.the;
	dr.heading = data.psi;
	dr.roll = data.phi;

	clock_t timeNow = (clock()*60/CLOCKS_PER_SEC)%90+id*10;
	float touchDownSmoke=0.0;
	if(clock()/CLOCKS_PER_SEC<(touchDownTime+3))
		touchDownSmoke=1.0;
	float thrust=timeNow+171.0;
	float gear=data.gear_deploy;

	if(velocity/velocity>0.0f){
		if(rpm<=100){
			//begin move
			startMoveTime=clock()/CLOCKS_PER_SEC*1.0f; 
			rpm=110;
		}
		if(rpm!=1500){
			double nowT=clock()/(CLOCKS_PER_SEC*1.0f);
			if(nowT<(startMoveTime+1)){
				
        		double change=((nowT-startMoveTime)/1.0f);
				rpm=100+1400*change;
				
			}
			else
				rpm=1500;
		}
	}else{
		if(rpm==1500){
			startMoveTime=clock()/CLOCKS_PER_SEC*1.0f; 
			rpm=1400;
		}
		if(rpm!=100){
			double nowT=clock()/(CLOCKS_PER_SEC*1.0f);
			if(nowT<(startMoveTime+1)){
				
        		double change=((nowT-startMoveTime)/1.0f);
				rpm=1500-1400*change;
				
			}
			else{
				if(data.engineoff)
					rpm=0;
				else
					rpm=100;
			}
		}
	}

	thrust*=rpm;

	

    if(g_instance[0])
    {

		if(ref_style==2){
			//float tire[19] = {0,0,gear,gear,0,1.0,1.0,gear*useNavLights,useNavLights,0,0,0,touchDownSmoke,1500,1500,thrust,thrust,NULL};
			
			float tire[19] = {0,0,gear,gear,0,1.0,1.0,gear*useNavLights,useNavLights,0,0,0,touchDownSmoke,(float)rpm,(float)rpm,thrust,thrust,NULL};
			XPLMInstanceSetPosition(g_instance[0], &dr, tire);
		}
		else{
			float tire[17] = {0,0,gear,gear,0,1.0,1.0,gear*useNavLights,useNavLights,0,0,0,touchDownSmoke,thrust,thrust,NULL};
			XPLMInstanceSetPosition(g_instance[0], &dr, tire);
		}
        
    }
}



void initPlanes(){

	soundSystem.showActive();

    gPlaneX = XPLMFindDataRef("sim/flightmodel/position/local_x");
	gPlaneY = XPLMFindDataRef("sim/flightmodel/position/local_y");
	gPlaneZ = XPLMFindDataRef("sim/flightmodel/position/local_z");
	gPlaneTheta = XPLMFindDataRef("sim/flightmodel/position/theta");
	gPlanePhi = XPLMFindDataRef("sim/flightmodel/position/phi");
	gPlanePsi = XPLMFindDataRef("sim/flightmodel/position/psi");
	gOverRidePlanePosition = XPLMFindDataRef("sim/operation/override/override_planepath");
	gAGL = XPLMFindDataRef("sim/flightmodel/position/y_agl");
	gdaylight = XPLMFindDataRef("sim/graphics/scenery/airport_lights_on");
	//XPLMRegisterFlightLoopCallback(	MyFlightLoopCallback0,/* Callback */-1.0,/* Interval */NULL);/* refcon not used. */
	BeginAI();
	XPLMRegisterFlightLoopCallback(	MyFlightLoopCallback,/* Callback */-1.0,	/* Interval */NULL);/* refcon not used. */
    for(int i=0;i<30;i++){
		aircraft[i].id=i+1;
	}

}
void stopPlanes(){
    //XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback0, NULL);
	XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, NULL);
    //XPLMReleasePlanes();//not used anymore
}
bool loaded=false;
/*float	MyFlightLoopCallback0(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon)*/
float	BeginAI()
{
	//int AircraftIndex;
    if(!loaded){


		char def[512]="Aircraft/Laminar Research/Boeing B747-400/747-400.acf";//"Resources/plugins/AutoATC/Aircraft/JD330/airserbia/a330.acf";
		char defModel[512];

		char xppath[512];
		XPLMGetSystemPath(xppath);
		sprintf (defModel, "%s%s", xppath, def);

        loaded=true;
		XPLMDebugString("AutoATC: started AI planes.\n");
       
        {
            XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"cjs/world_traffic/engine_rotation_angle1");
            XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"cjs/world_traffic/engine_rotation_angle2");
            XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"traf/flaps");
            XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"traf/lgear");
            XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"traf/nw_rot");
            XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"traf/bcn");
            XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"traf/strobe");
            XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"traf/land");
            XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"traf/nav");
            XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"traf/taxi");
            XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"traf/sb");   
        }
		
    }

   

	return 0;
}

AircraftData userData;
float	MyFlightLoopCallback(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon)
{
	
    
	// Get User Aircraft data
	userData.x = XPLMGetDataf(gPlaneX);
	userData.y = XPLMGetDataf(gPlaneY);
	userData.z = XPLMGetDataf(gPlaneZ);
	userData.the = XPLMGetDataf(gPlaneTheta);
	userData.phi = XPLMGetDataf(gPlanePhi);
	userData.psi = XPLMGetDataf(gPlanePsi);
	useNavLights = XPLMGetDatai(gdaylight)*1.0;
	
	for(int i=0;i<30;i++){
		aircraft[i].GetAircraftData(userData);
		aircraft[i].SetAircraftData();
	}
	soundSystem.update();
	return -1;
}
