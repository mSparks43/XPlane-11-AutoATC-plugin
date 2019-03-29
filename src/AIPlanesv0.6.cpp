
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
/*char xp_path[512];
AircraftFrame::AircraftFrame(char tmodel[512],double tyOffset){
	strcpy(model,tmodel);
	yOffset=tyOffset;
};
char* AircraftFrame::getModel(){
	return model;
};
double AircraftFrame::getOffset(){
	return yOffset;
}
AircraftFrames::AircraftFrames(){
	//airframes=AircraftFrame[4];
	char def[512]="Resources/plugins/AutoATC/Aircraft/JD330/airserbia/a330.acf";
	char defModel[512];
	XPLMGetSystemPath(xp_path);
	sprintf (defModel, "%s%s", xp_path, def);
	//XPLMDebugString(defModel);
	airframes[0]=new AircraftFrame(defModel,3.5);
	char def2[512]="Resources/plugins/AutoATC/Aircraft/JD330/airberlin/a320neo.acf";
	char defModel2[512];
	XPLMGetSystemPath(xp_path);
	sprintf (defModel2, "%s%s", xp_path, def2);
	//XPLMDebugString(defModel2);
	airframes[1]=new AircraftFrame(defModel2,2.8);
	airframes[2]=new AircraftFrame(defModel2, 2.8);
	char def3[512]="Aircraft/Robinson R44 Raven II/r44.acf";
	char defModel3[512];
	XPLMGetSystemPath(xp_path);
	sprintf (defModel3, "%s%s", xp_path, def3);
	airframes[3]=new AircraftFrame(defModel3,1.0);
	//airframes[3]=new AircraftFrame("/media/storage2/X-Plane 11/X-Plane 11/Aircraft/Laminar Research/Boeing B737-800/b738.acf",2.9);
	airframes[4]=new AircraftFrame("/media/storage2/X-Plane 11/X-Plane 11/Aircraft/Laminar Research/Boeing B747-400/747-400.acf",5.0);
};
AircraftFrame* AircraftFrames::getFrame(int id){
	return airframes[id];
}

AircraftFrames Airframes;*/
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

static float	MyFlightLoopCallback0(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon);    

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
	/*char	x_str[80];
	char	y_str[80];
	char	z_str[80];
	char	the_str[80];
	char	phi_str[80];
	char	psi_str[80];
	char	gear_deploy_str[80];
	char	throttle_str[80];*/
    
	/*strcpy(x_str, "sim/multiplayer/position/planeX_x");
	strcpy(y_str,	"sim/multiplayer/position/planeX_y");
	strcpy(z_str,	"sim/multiplayer/position/planeX_z");
	strcpy(the_str,	"sim/multiplayer/position/planeX_the");
	strcpy(phi_str,	"sim/multiplayer/position/planeX_phi");
	strcpy(psi_str,	"sim/multiplayer/position/planeX_psi");
	strcpy(gear_deploy_str,	"sim/multiplayer/position/planeX_gear_deploy");
	strcpy(throttle_str, "sim/multiplayer/position/planeX_throttle");
    
	char cTemp = (AircraftNo + 0x30);
	x_str[30]			=	cTemp;
	y_str[30]			=	cTemp;
	z_str[30]			=	cTemp;
	the_str[30]			=	cTemp;
	phi_str[30]			=	cTemp;
	psi_str[30]			=	cTemp;
	gear_deploy_str[30] =	cTemp;
	throttle_str[30]	=	cTemp;

	dr_plane_x				= XPLMFindDataRef(x_str);
	dr_plane_y				= XPLMFindDataRef(y_str);
	dr_plane_z				= XPLMFindDataRef(z_str);
	dr_plane_the			= XPLMFindDataRef(the_str);
	dr_plane_phi			= XPLMFindDataRef(phi_str);
	dr_plane_psi			= XPLMFindDataRef(psi_str);
	dr_plane_gear_deploy	= XPLMFindDataRef(gear_deploy_str);
	dr_plane_throttle		= XPLMFindDataRef(throttle_str);*/
	//id=AircraftNo;
	ground_probe = XPLMCreateProbe(xplm_ProbeY);
	yOffset=5.0;//yOffsetp;
	airFrameIndex=-1;
	toLoadAirframe=false;
}
  char gBob_debstr3[128];
 /* static void load_cb(const char * real_path, void * ref)
{
	XPLMObjectRef * dest = (XPLMObjectRef *) ref;
	if(*dest == NULL)
	{
		*dest = XPLMLoadObject(real_path);
		printf("loaded object %s\n",real_path);
	}
	else
		printf("skipped loading object %s\n",real_path);
}*/
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
	/*const char * drefs[15]={ "traf/mw_def","traf/nw_def","traf/lgear","traf/flaps","traf/nw_rot","traf/bcn","traf/strobe"
	,"traf/land","traf/nav","traf/taxi","traf/sb","traf/vint","traf/touch_down","traf/vint_circle", NULL };
	XPLMCreateInstance(inObject, drefs);*/
	inLoading=false;
	
}
int pid=25;
char found_path[1024];
 static void load_cb(const char * real_path, void * ref)
{
	XPLMObjectRef * dest = (XPLMObjectRef *) ref;
	if(*dest == NULL)
	{
		//*dest = XPLMLoadObject(real_path);
		//planeid id;
		//found_path=*real_path;
		/*int *id=&pid;

		XPLMLoadObjectAsync(real_path, loadedobject, id);*/
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
				//*g_object=inObject;
	            //const char * drefs[] = { "sim/graphics/animation/ground_traffic/tire_steer_deg", NULL };
		       /* if(!g_instance[0])
		        {
					if(!wt3)
			        	g_instance[0] = XPLMCreateInstance(inObject, cls_drefs);
					else{
						printf("is wt3");
						g_instance[0] = XPLMCreateInstance(inObject, wt3_drefs);
					}
		        }
				else{
					XPLMDestroyInstance(g_instance[0]);
					if(!wt3)
						g_instance[0] = XPLMCreateInstance(inObject, cls_drefs);
					else
						g_instance[0] = XPLMCreateInstance(inObject, wt3_drefs);
				}*/
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
	/*plane_x = XPLMGetDataf(dr_plane_x);
	plane_y = XPLMGetDataf(dr_plane_y);
	plane_z = XPLMGetDataf(dr_plane_z);
	plane_the = XPLMGetDataf(dr_plane_the);
	plane_phi = XPLMGetDataf(dr_plane_phi);
	plane_psi = XPLMGetDataf(dr_plane_psi);
	XPLMGetDatavf(dr_plane_gear_deploy, plane_gear_deploy, 0, 5);
	XPLMGetDatavf(dr_plane_throttle, plane_throttle, 0, 8);*/
    // Copy it to each aircraft using different offsets for each aircraft.
    
    PlaneData thisData;
	
	
	clock_t timeNow = clock();
	
	//double ttN=((float)nextData.time-(float)timeNow)/CLOCKS_PER_SEC;
	//if(nextData.time!=lastTime)
	//	htWindow=(float)nextData.time-(float)lastTime;
	//if(duration<htWindow && ttN>0 && ttN>htWindow)
    

	JVM* jvmO;
	try{
		jvmO=getJVM();
		thisData=jvmO->getPlaneData(id);
	}catch(...){
		XPLMDebugString("Exception getting plane data");
		return;
	}
	
	if(!thisData.live){
		//data.live=false;
		/*lastData.x=nextData.x=data.x = XPLMGetDataf(dr_plane_x)+100000;
		lastData.x=nextData.y=data.y = XPLMGetDataf(dr_plane_y)+6000;
		lastData.x=nextData.z=data.z = XPLMGetDataf(dr_plane_z)+100000;
		data.the = XPLMGetDataf(dr_plane_the);
		data.phi = XPLMGetDataf(dr_plane_phi);
		data.psi = XPLMGetDataf(dr_plane_psi);*/
		lastData.time=timeNow;
		nextData.time=timeNow;
		
		        if(g_instance[0])
		        {
					printf("dropping object for id= %d\n",id);
					XPLMDestroyInstance(g_instance[0]);
					g_instance[0]=NULL;
					airFrameIndex=-1;
					//g_instance[0] = XPLMCreateInstance(g_object, drefs);
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
			//sprintf(gBob_debstr3,af);
			//XPLMSetAircraftModel(id,gBob_debstr3);
           /* if(!g_object)
	        {
                 XPLMDebugString("AutoATC: lookup object.\n");
               // const char * g_objPath = "BluebellXL/A320_AAR.obj";
                //const char * g_objPath = "lib/airport/vehicles/pushback/tug.obj";
				//char* af_p=gBob_debstr3;

		        XPLMLookupObjects(af, 0, 0, load_cb, &g_object);
                //XPLMLoadObject("/media/storage2/X-Plane 11/X-Plane 11_11.30/Custom Scenery/Bluebell XL/PackA/A320/A320_AAF.obj");
	        }
			else{
				//we already loaded a model, delete it
				XPLMDebugString("AutoATC: drop object.\n");
				XPLMUnloadObject(g_object);
				XPLMLookupObjects(af, 0, 0, load_cb, &g_object);
			}
            if(g_object)
	        {
	            //const char * drefs[] = { "sim/graphics/animation/ground_traffic/tire_steer_deg", NULL };
				printf("using existing object for %d to %s sound=%d\n",id,af,soundIndex);
		        if(!g_instance[0])
		        {
			        //g_instance[0] = XPLMCreateInstance(g_object, drefs);
					if(!wt3)
						g_instance[0] = XPLMCreateInstance(g_object, cls_drefs);
					else
						g_instance[0] = XPLMCreateInstance(g_object, wt3_drefs);
		        }
				else{
					XPLMDestroyInstance(g_instance[0]);
					//g_instance[0] = XPLMCreateInstance(g_object, drefs);
					if(!wt3)
						g_instance[0] = XPLMCreateInstance(g_object, cls_drefs);
					else
						g_instance[0] = XPLMCreateInstance(g_object, wt3_drefs);
				}
            }
            else
			{
				/*sprintf (gBob_debstr3, "id %d at %s.\n", thisData.airframe,af);
				XPLMDebugString("AutoATC: failed lookup object ");
            	XPLMDebugString(gBob_debstr3);*/
				//printf ("id %d at %s.\n", id,*found_path);
				//const char real_path=*found_path;
				int *pid=&id;
				inLoading=true;
				XPLMLoadObjectAsync(af, loadedobject, pid);
			//}
	}
	//char debug[512];
	//sprintf(debug,"%i data has af %i wants %i\r\n",id,airFrameIndex,thisData.airframe);
	//XPLMDebugString(debug);
    if(lastData.time==nextData.time){
        //if this is the first data point we have, just set it, and come back later
		//sprintf(debug,"%i first data live %lu time %lu clocks per sec %f\r\n",id,clock(),CLOCKS_PER_SEC,thisData.timeStamp);
		//XPLMDebugString(debug);
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
	
	//sprintf(debug,"%i live 2 %lu time last %f this %f\r\n",id,clock(),lastData.remoteTimestamp,thisData.timeStamp);
	//XPLMDebugString(debug);
    if(lastData.remoteTimestamp==(double)thisData.timeStamp){
        //we dont have new data
        return;
    }
	//sprintf(debug,"%i live 3 %lu time %lu clocks per sec\r\n",id,clock(),CLOCKS_PER_SEC);
	//XPLMDebugString(debug);
    if(nextData.time<lastData.time){
        //first time we have nextData
		//sprintf(debug,"%i first next data %lu time %lu clocks per sec\r\n",id,clock(),CLOCKS_PER_SEC);
		//XPLMDebugString(debug);
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
    
    
    /*(duration>htWindow){
        data.x = nextData.x;
		data.y = nextData.y;
		data.z = nextData.z;

        data.the=nextData.the;
        data.phi=nextData.phi;
        data.psi=nextData.psi;
    }
    else*/
	//{
		//fill in the gaps here
		//XPLMDebugString("Interpolating data");
		double pComplete=duration/(htWindow);
		float vx=nextData.x-lastData.x;
		float vy=nextData.y-lastData.y;
		float vz=nextData.z-lastData.z;
		velocity=v((nextData.x-lastData.x)/htWindow,(nextData.y-lastData.y)/htWindow,(nextData.z-lastData.z)/htWindow);
        /*float vh=nextData.the-lastData.the;
		float vp=nextData.phi-lastData.phi;
		float vs=nextData.psi-lastData.psi;*/

		float x=lastData.x+(vx*pComplete);
		float y=lastData.y+(vy*pComplete);
		float z=lastData.z+(vz*pComplete);

        /*float h=lastData.the+(vh*pComplete);
		float p=lastData.phi+(vp*pComplete);
		float s=lastData.psi+(vs*pComplete);*/

		data.x = x;
		data.y = y;
		data.z = z;

        data.the=thisData.the;
        data.phi=thisData.phi;
        data.psi=thisData.psi;
		//return;
	//}
    if(nextData.remoteTimestamp<(thisData.timeStamp-300)){
        //we have a new position to get to
        //set last data to where we are now

        lastData.remoteTimestamp= nextData.remoteTimestamp;//+=(duration*1000);
		//lastData.remoteTimestamp=nextData.remoteTimestamp+(duration)*1000
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
	//sprintf(gBob_debstr3,"agl %d=%f!\n",id,requestedAGL);
    //XPLMDebugString(gBob_debstr3);
	/*if(requestedAGL<18.0){
		data.y=outInfo.locationY;

	}
	else if(requestedAGL<40.0){
		requestedAGL=((requestedAGL-18.0)*2);
		data.y=outInfo.locationY+requestedAGL;
	}
	data.y=data.y+yOffset; //was data*/

	//data.the = thisData.the;
	//data.phi = thisData.phi;
	//data.psi = thisData.psi;
	double gearDown=0.0;
	if(requestedAGL<400)
		gearDown=1.0;
	else if(requestedAGL<500){
		gearDown=(100.0f-(requestedAGL-400.0f))/100.0f;
	}	
	for (int Gear=0; Gear<5; Gear++)
	{
		data.gear_deploy[Gear] = gearDown;//thisData.gearDown;
		
	}
    for (int Throttle=0; Throttle<8; Throttle++)
	{
		data.throttle[Throttle] = 0.0;//thisData.throttle;
		
	}

	/*if(ttN>htWindow){
		//update last Data
		//lastData.time=timeNow;
		
		return;
	}else*/
    {
	/*	lastData.x=nextData.x;
		lastData.y=nextData.y;
		lastData.z=nextData.z;
		lastData.time=timeNow-(2*htWindow*CLOCKS_PER_SEC);//nextData.time;

		nextData.x=thisData.x;
		nextData.y=thisData.y;
		nextData.z=thisData.z;
		nextData.time=timeNow+(2*htWindow*CLOCKS_PER_SEC);*/
		XPLMProbeInfo_t outInfo;
		outInfo.structSize = sizeof(outInfo);
		XPLMProbeResult ground= XPLMProbeTerrainXYZ(ground_probe,data.x,data.y,data.z,&outInfo);
		double requestedAGL=data.y-outInfo.locationY;
		//sprintf(gBob_debstr3,"agl %d=%f!\n",id,requestedAGL);
    	//XPLMDebugString(gBob_debstr3);
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
		//nextData.y=nextData.y; //was data +yOffset
		if( data.the>0){
			double lift=15.0*tan(data.the*PI/180.0);
			data.y=data.y+lift;
		}
		//check model
		if(airFrameIndex==-1&&airFrameIndex!=thisData.airframe){
			//thisData.airframe=586;//378
			//AircraftFrame* newFrame=Airframes.getFrame(thisData.airframe);
			soundIndex=jvmO->getSound(thisData.airframe);
			yOffset=jvmO->getOffset(thisData.airframe);
			//if(yOffset!=0.0)
			//wt3=(yOffset!=0.0);
			ref_style=jvmO->getDrefStyle(thisData.airframe);
			sprintf(gBob_debstr3,"AutoATC switch Airframe %d. at %f style=%d\n",thisData.airframe,yOffset,ref_style);
        	XPLMDebugString(gBob_debstr3);
        	//XPLMDebugString(gBob_debstr3);
			//XPLMDebugString(af);
			airFrameIndex=thisData.airframe;
			//char* af=jvmO->getModel(954);//thisData.airframe);//newFrame->getModel();//.airframes[thisData.airframe].model;
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
	/*XPLMSetDataf(dr_plane_x, data.x);
	XPLMSetDataf(dr_plane_y, data.y);
	XPLMSetDataf(dr_plane_z, data.z);
	XPLMSetDataf(dr_plane_the, data.the);
	XPLMSetDataf(dr_plane_phi, data.phi);
	XPLMSetDataf(dr_plane_psi, data.psi);
	XPLMSetDatavf(dr_plane_gear_deploy, data.gear_deploy, 0, 5);
	XPLMSetDatavf(dr_plane_throttle,data.throttle, 0, 8);*/
    XPLMDrawInfo_t		dr;
	dr.structSize = sizeof(dr);
	dr.x = data.x;
	dr.y = data.y+yOffset;
	dr.z = data.z;
	dr.pitch = data.the;
	dr.heading = data.psi;
	dr.roll = data.phi;
    //static float tire[16] = {1-data.gear_deploy[0],0,0,0,0,0,0,0,0,0,0,0,0,0,0,NULL};
	clock_t timeNow = (clock()*60/CLOCKS_PER_SEC)%90+id*10;
	float touchDownSmoke=0.0;
	if(clock()/CLOCKS_PER_SEC<(touchDownTime+3))
		touchDownSmoke=1.0;
	float thrust=timeNow+171.0;
	float gear=data.gear_deploy[0];
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
			else
				rpm=100;
		}
	}

	thrust*=rpm;
	/*if(ref_style==2)
	{
		gear=1-gear;
		thrust=0.0;
	}*/
	//float tire[17] = {0,0,gear,gear,0,1.0,1.0,gear*useNavLights,useNavLights,0,0,0,touchDownSmoke,thrust,thrust,NULL};

	//tire += 10.0;
	//if(tire > 45.0) tire -= 90.0;
	

    if(g_instance[0])
    {

		if(ref_style==2){
			//float tire[19] = {0,0,gear,gear,0,1.0,1.0,gear*useNavLights,useNavLights,0,0,0,touchDownSmoke,1500,1500,thrust,thrust,NULL};
			
			float tire[19] = {0,0,gear,gear,0,1.0,1.0,gear*useNavLights,useNavLights,0,0,0,touchDownSmoke,rpm,rpm,thrust,thrust,NULL};
			XPLMInstanceSetPosition(g_instance[0], &dr, tire);
		}
		else{
			float tire[17] = {0,0,gear,gear,0,1.0,1.0,gear*useNavLights,useNavLights,0,0,0,touchDownSmoke,thrust,thrust,NULL};
			XPLMInstanceSetPosition(g_instance[0], &dr, tire);
		}
        
    }
}
//AircraftFrames Airframes();
/*Aircraft Aircraft1(1);
Aircraft Aircraft2(2);
Aircraft Aircraft3(3);
Aircraft Aircraft4(4);
Aircraft Aircraft5(5);
Aircraft Aircraft6(6);
Aircraft Aircraft7(7);
Aircraft Aircraft8(8);*/


void initPlanes(){
    //XPLMGetSystemPath(xp_path);
    
    //int r = XPLMAcquirePlanes(NULL, NULL, NULL);
	soundSystem.showActive();
	/*if(r == 0)	{
        XPLMDebugString("AutoATC: did not acquire planes.\n");
        return;
    }*/
	//XPLMSetActiveAircraftCount(9);
    //XPLMDebugString("AutoATC: Acquired planes.\n");
    gPlaneX = XPLMFindDataRef("sim/flightmodel/position/local_x");
	gPlaneY = XPLMFindDataRef("sim/flightmodel/position/local_y");
	gPlaneZ = XPLMFindDataRef("sim/flightmodel/position/local_z");
	gPlaneTheta = XPLMFindDataRef("sim/flightmodel/position/theta");
	gPlanePhi = XPLMFindDataRef("sim/flightmodel/position/phi");
	gPlanePsi = XPLMFindDataRef("sim/flightmodel/position/psi");
	gOverRidePlanePosition = XPLMFindDataRef("sim/operation/override/override_planepath");
	gAGL = XPLMFindDataRef("sim/flightmodel/position/y_agl");
	gdaylight = XPLMFindDataRef("sim/graphics/scenery/airport_lights_on");
	XPLMRegisterFlightLoopCallback(	MyFlightLoopCallback0,/* Callback */-1.0,/* Interval */NULL);/* refcon not used. */

	XPLMRegisterFlightLoopCallback(	MyFlightLoopCallback,/* Callback */-1.0,	/* Interval */NULL);/* refcon not used. */
    for(int i=0;i<30;i++){
		aircraft[i].id=i+1;
	}

}
void stopPlanes(){
    XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback0, NULL);
	XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, NULL);
    XPLMReleasePlanes();
}
bool loaded=false;
float	MyFlightLoopCallback0(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon)
{
	int AircraftIndex;
    if(!loaded){
        /*XPLMSetAircraftModel(1,"/media/storage2/X-Plane 11/X-Plane 11/Aircraft/Laminar Research/Boeing B737-800/b738.acf");2.9
        XPLMSetAircraftModel(2,"/media/storage2/X-Plane 11/X-Plane 11/Aircraft/Laminar Research/Boeing B747-400/747-400.acf");5.0
        XPLMSetAircraftModel(3,"/media/storage2/X-Plane 11/X-Plane 11/Aircraft/Laminar Research/Boeing B737-800/b738.acf");
        XPLMSetAircraftModel(4,"/media/storage2/X-Plane 11/X-Plane 11/Aircraft/Laminar Research/Boeing B737-800/b738.acf");
        XPLMSetAircraftModel(5,"/media/storage2/X-Plane 11/X-Plane 11/Aircraft/Laminar Research/Boeing B737-800/b738.acf");
        XPLMSetAircraftModel(6,"/media/storage2/X-Plane 11/X-Plane 11/Aircraft/Laminar Research/Boeing B737-800/b738.acf");
        XPLMSetAircraftModel(7,"/media/storage2/X-Plane 11/X-Plane 11/Aircraft/Laminar Research/Boeing B737-800/b738.acf");
        XPLMSetAircraftModel(8,"/media/storage2/X-Plane 11/X-Plane 11/Aircraft/Laminar Research/Boeing B737-800/b738.acf");*/
		//char def[512]="Aircraft/Laminar Research/Boeing B737-800/b738.acf";
		char def[512]="Aircraft/Laminar Research/Boeing B747-400/747-400.acf";//"Resources/plugins/AutoATC/Aircraft/JD330/airserbia/a330.acf";
		char defModel[512];

		char xppath[512];
		XPLMGetSystemPath(xppath);
		sprintf (defModel, "%s%s", xppath, def);
		/*XPLMSetAircraftModel(1,defModel);//2.9
        XPLMSetAircraftModel(2,defModel);
        XPLMSetAircraftModel(3,defModel);
        XPLMSetAircraftModel(4,defModel);
        XPLMSetAircraftModel(5,defModel);
        XPLMSetAircraftModel(6,defModel);
        XPLMSetAircraftModel(7,defModel);
        XPLMSetAircraftModel(8,defModel);*/
        loaded=true;
		XPLMDebugString("AutoATC: started AI planes.\n");
        /*XPLMPluginID PluginID = XPLMFindPluginBySignature("zem.AutoATC.RadioTransciever");
        if (PluginID != XPLM_NO_PLUGIN_ID)*/
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
		//Airframes=new AircraftFrames();
    }
	// Disable AI for each aircraft.
	/*for (AircraftIndex=1; AircraftIndex<10; AircraftIndex++)
		XPLMDisableAIForPlane(AircraftIndex); */   
   

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
