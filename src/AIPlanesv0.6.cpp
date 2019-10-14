
#include <string.h>
#include <math.h>
//#include <cmath.h>
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
#include "jvm.h"
#include "Simulation.h"
 #include "aiplane.h"


  #include "AISound.h"
 #include <thread>
//#include <mutex>
#include <chrono>
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

static float	AircraftLoopCallback(
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
	ll=new Simulation(0.01);
	rp=new Simulation(0.01);
	ahs=new Simulation(0.01);
	thisData.live=false;
	thisData.x=0.0;
    thisData.y=0.0;
    thisData.z=0.0;
    thisData.gearDown=0.0;
    thisData.throttle=0.0;
    thisData.the=0.0;
	thisData.phi=0.0;
	thisData.psi=0.0;
    thisData.timeStamp=0.0;
    thisData.airframe=-1;
	
}
  char gBob_debstr3[128];
 
Aircraft aircraft[30];
AircraftSounds soundSystem(aircraft);
//planeid *plnid=NULL;
bool inLoading;
static void loadedobject(XPLMObjectRef inObject, void *inRef){
	int id=*(int *)inRef;
	char debugStr[512];
	sprintf(debugStr,"loaded object C++id=%d ref=%d\n",id,inObject);
	XPLMDebugString(debugStr);
	printf(debugStr);
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
//void Aircraft::GetAircraftData(AircraftData userdata)
void Aircraft::GetAircraftData(){
	JVM* jvmO;
	try{
		jvmO=getJVM();
		
		PlaneData newData=jvmO->getPlaneData(id,jvmO->env);
		//thisData.live=newData.live;
		//if(!newData.live)
		//	return;
		//printf("wtf %d is live? has airframe %d\n",id,newData.airframe);
	}catch(...){
		XPLMDebugString("Exception getting plane data");
		return;
	}
}
void Aircraft::GetAircraftThreadData(){
	JVM* jvmO;
	try{
		jvmO=getJVM();
		
		PlaneData newData=jvmO->getPlaneData(id,jvmO->plane_env);
		
		data_mutex.lock();
		thisData.live=newData.live;
		thisData.airframe=newData.airframe;
		if(!newData.live){
			data_mutex.unlock();
			return;
		}
			
		thisData.x=newData.x;
    	thisData.y=newData.y;
   		thisData.z=newData.z;
    	thisData.gearDown=newData.gearDown;
    	thisData.throttle=newData.throttle;
    	thisData.the=newData.the;
		thisData.phi=newData.phi;
		thisData.psi=newData.psi;
    	thisData.timeStamp=newData.timeStamp;
		clock_t timeNow = clock();
		if(lastData.time==nextData.time){
			lastData.remoteTimestamp=thisData.timeStamp;
			lastData.time=timeNow;
			lastData.x=thisData.x;
			lastData.y=thisData.y;
			lastData.z=thisData.z;
			lastData.the=thisData.the;
			lastData.phi=thisData.phi;
			lastData.psi=thisData.psi;
			//printf("init plane location %d",id);
			data_mutex.unlock();
			return;
		}

		if(lastData.remoteTimestamp==thisData.timeStamp){
			//we dont have new data
			data_mutex.unlock();
			return;
		}
		
	
			lastData.remoteTimestamp= nextData.remoteTimestamp;
			lastData.time=timeNow;
			lastData.x=nextData.x;
			lastData.y=nextData.y;
			lastData.z=nextData.z;
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
			data_mutex.unlock();
		//}
	}catch(...){
		XPLMDebugString("Exception getting plane data");
		return;
	}
}
void Aircraft::PrepareAircraftData()
{
   
    
	
	
	data_mutex.lock();
	JVM* jvmO=getJVM();

    

	
	
	if(!thisData.live){
		        if(g_instance[0])
		        {
					printf("dropping object for id= %d\n",(id-1));
					XPLMDestroyInstance(g_instance[0]);
					g_instance[0]=NULL;
					airFrameIndex=-1;
				}
		data_mutex.unlock();
		return;
	}
	
	if(!inLoading&&toLoadAirframe){
		toLoadAirframe=false;
		char* af=jvmO->getModel(airFrameIndex);
		char debugStr[512];
		if(g_instance[0])
	    {
			printf("dropping object for load id= %d\n",id);
			XPLMDestroyInstance(g_instance[0]);
					
		}
		XPLMDebugString("toLoadAirframe\n");
		sprintf(debugStr,"searching for id=%d afI=%d to %s sound=%d\n",id,airFrameIndex,af,soundIndex);
		XPLMDebugString(debugStr);
		int *pid=&id;
		inLoading=true;
		XPLMLoadObjectAsync(af, loadedobject, pid);
		
	}

   
    clock_t timeNow = clock();
	clock_t lastTime= lastData.time;
	double duration=((double)timeNow-(double)lastTime)/CLOCKS_PER_SEC;
	if(duration<0)
		duration=0;
    double htWindow=(nextData.remoteTimestamp-lastData.remoteTimestamp)/1000.0;//the time it should take us in seconds
    if(htWindow<1)
		htWindow=1;

	double pComplete=duration/(htWindow);
	double vx=nextData.x-lastData.x;
	double vy=nextData.y-lastData.y;
	double vz=nextData.z-lastData.z;
	velocity=v((nextData.x-lastData.x)/htWindow,(nextData.y-lastData.y)/htWindow,(nextData.z-lastData.z)/htWindow);


	double x=lastData.x+(vx*pComplete);
	double y=lastData.y+(vy*pComplete);
	double z=lastData.z+(vz*pComplete);


	data.x = x;
	data.y = y;
	data.z = z;
	data.the=thisData.the;
    data.phi=thisData.phi;
	if(thisData.gearDown==2.0){
		data.engineoff=true;
	}
	else{
		data.engineoff=false;//false;
	}
    
	XPLMProbeInfo_t outInfo;
	outInfo.structSize = sizeof(outInfo);
	//XPLMProbeResult ground= 
	XPLMProbeTerrainXYZ(ground_probe,data.x,data.y,data.z,&outInfo);
	double requestedAGL=data.y-outInfo.locationY;
	double targetAGL=ahs->getX()-outInfo.locationY;
	double gearDown=0.0;
	if(requestedAGL<200)
		gearDown=1.0;
	else if(requestedAGL<250){
		gearDown=(50.0f-(requestedAGL-200.0f))/50.0f;
	}	
	{
		data.gear_deploy = gearDown;//thisData.gearDown;
		
	}
    for (int Throttle=0; Throttle<8; Throttle++)
	{
		data.throttle[Throttle] = 0.0;//thisData.throttle;
		
	}

	
    {
	
		
		if(requestedAGL<18.0){
			data.y=outInfo.locationY;
			if(inTouchDown&&targetAGL<0.5){
				touchDownTime=clock()/CLOCKS_PER_SEC;
				soundSystem.land(id-1);
				inTouchDown=false;
			}
			data.the=0.0;
    		data.phi=0.0;
			if(soundIndex==2){//Heli
				float speed=velocity/velocity;
				double now=((double)clock())/(CLOCKS_PER_SEC*1.0f);
				if(speed>0.1f){
					
					if(now<(startMoveTime+3))
					{
						double sComplete=(now-startMoveTime)/3.0;
						//printf("lift off %f %f\n",sComplete,startMoveTime);
						if(sComplete>0.5f)
							inHover=true;
						data.y+=(2.0*sComplete);
						data.the-=(2.0*sComplete);
					}
					else if(speed<5&&rpm==1500){//we're moving but in the frame before we ascend
						data.y+=2.0;//always air taxi
						data.the-=2.0;//lift nose
					}else if(rpm>100){
						//inHover=true;
						data.y+=2;//always air taxi
						data.the-=3.0;//drop nose
					}
					
				}
				else if(now<(startMoveTime+3))
				{
						double sComplete=1.0-((now-startMoveTime)/3.0);
						if(sComplete<0.5f)
							inHover=false;
						//printf("land %f %f\n",sComplete,startMoveTime);
						data.y+=(2.0*sComplete);
						data.the=0.0;
				}
				else if(inHover){//we're stopped, but in the frame before we descend
					data.y+=2.0;
					//printf("ready land %f\n",startMoveTime);
				}
				if(data.the>-1.0)
					data.the=-1.0;
			}
		}
		else if(requestedAGL<=198.0){
			//data.the=0.0;
    		data.phi=thisData.phi/3;
			if(soundIndex!=2)//no screech for helos
				inTouchDown=true;
			requestedAGL=((requestedAGL-18.0)*1.1);
			if(soundIndex==2){
				requestedAGL+=2;
				if(data.the>-1.0){
					data.the=-1.0;
					data.phi=thisData.phi/3;
				}
			}
			data.y=outInfo.locationY+requestedAGL;
		}else if(soundIndex==2){
			if(data.the>-1.0)
				data.the=-1.0;
			data.y+=2;		
		}

		if( data.the>0){
			double lift=25.0*tan(data.the*PI/180.0);
			data.y=data.y+lift;
		}
		//check model
		if((airFrameIndex==-1&&airFrameIndex!=thisData.airframe)||(thisData.airframe>0&&airFrameIndex!=thisData.airframe)){

			soundIndex=jvmO->getSound(thisData.airframe);
			yOffset=jvmO->getOffset(thisData.airframe);

			ref_style=jvmO->getDrefStyle(thisData.airframe);
			sprintf(gBob_debstr3,"AutoATC switch Airframe from %d to %d. at %f style=%d\n",airFrameIndex,thisData.airframe,yOffset,ref_style);
        	XPLMDebugString(gBob_debstr3);
			printf(gBob_debstr3);
			airFrameIndex=thisData.airframe;
			toLoadAirframe=true;
			
		}
	}
	data_mutex.unlock();
}
/*void Aircraft::PrepareAircraftData()
{
   
    
	
	
	clock_t timeNow = clock();
	JVM* jvmO=getJVM();

    

	
	
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
		if(g_instance[0])
	    {
			printf("dropping object for load id= %d\n",id);
			XPLMDestroyInstance(g_instance[0]);
					
		}
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
	double duration=((double)timeNow-(double)lastTime)/CLOCKS_PER_SEC;
	if(duration<0)
		duration=0;
    double htWindow=(nextData.remoteTimestamp-lastData.remoteTimestamp)/1000.0;//the time it should take us in seconds
    if(htWindow<1)
		htWindow=1;

		double pComplete=duration/(htWindow);
		double vx=nextData.x-lastData.x;
		double vy=nextData.y-lastData.y;
		double vz=nextData.z-lastData.z;
		velocity=v((nextData.x-lastData.x)/htWindow,(nextData.y-lastData.y)/htWindow,(nextData.z-lastData.z)/htWindow);


		double x=lastData.x+(vx*pComplete);
		double y=lastData.y+(vy*pComplete);
		double z=lastData.z+(vz*pComplete);


		data.x = x;
		data.y = y;
		data.z = z;

        data.the=thisData.the;
        data.phi=thisData.phi;
        //data.psi=thisData.psi;
		if(thisData.gearDown==2.0){
			//printf("engine off\n");
			data.engineoff=true;
		}
		else{
			//printf("engine on\n");
			data.engineoff=false;//false;
		}
    if(nextData.remoteTimestamp<(thisData.timeStamp-50)){
        //we have a new position to get to
        //set last data to where we are now

        lastData.remoteTimestamp= nextData.remoteTimestamp;
        lastData.time=timeNow;
        lastData.x=nextData.x-(vx*(1.0-pComplete));
		lastData.y=nextData.y-(vy*(1.0-pComplete));
		lastData.z=nextData.z-(vz*(1.0-pComplete));
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
	//double requestedAGL=data.y-outInfo.locationY;

	double requestedAGL=data.y-outInfo.locationY;
	double targetAGL=ahs->getX()-outInfo.locationY;
	double gearDown=0.0;
	if(requestedAGL<200)
		gearDown=1.0;
	else if(requestedAGL<250){
		gearDown=(50.0f-(requestedAGL-200.0f))/50.0f;
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
	
		
		if(requestedAGL<18.0){
			data.y=outInfo.locationY;
			if(inTouchDown&&targetAGL<0.5){
				touchDownTime=clock()/CLOCKS_PER_SEC;
				soundSystem.land(id-1);
				inTouchDown=false;
			}
			
			if(soundIndex==2){//Heli
				float speed=velocity/velocity;
				double now=((double)clock())/(CLOCKS_PER_SEC*1.0f);
				if(speed>0.1f){
					
					if(now<(startMoveTime+3))
					{
						double sComplete=(now-startMoveTime)/3.0;
						//printf("lift off %f %f\n",sComplete,startMoveTime);
						if(sComplete>0.5f)
							inHover=true;
						data.y+=(2.0*sComplete);
						data.the-=(2.0*sComplete);
					}
					else if(speed<5&&rpm==1500){//we're moving but in the frame before we ascend
						data.y+=2.0;//always air taxi
						data.the-=2.0;//lift nose
					}else if(rpm>100){
						//inHover=true;
						data.y+=2;//always air taxi
						data.the-=3.0;//drop nose
					}
					
				}
				else if(now<(startMoveTime+3))
				{
						double sComplete=1.0-((now-startMoveTime)/3.0);
						if(sComplete<0.5f)
							inHover=false;
						//printf("land %f %f\n",sComplete,startMoveTime);
						data.y+=(2.0*sComplete);
						data.the=0.0;
				}
				else if(inHover){//we're stopped, but in the frame before we descend
					data.y+=2.0;
					//printf("ready land %f\n",startMoveTime);
				}
				if(data.the>-1.0)
					data.the=-1.0;
			}
		}
		else if(requestedAGL<=198.0){
			if(soundIndex!=2)//no screech for helos
				inTouchDown=true;
			requestedAGL=((requestedAGL-18.0)*1.1);
			if(soundIndex==2){
				requestedAGL+=2;
				if(data.the>-1.0)
					data.the=-1.0;
			}
			data.y=outInfo.locationY+requestedAGL;
		}else if(soundIndex==2){
			if(data.the>-1.0)
				data.the=-1.0;
			data.y+=2;		
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
}*/
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
	if(!thisData.live)
		return;
    XPLMDrawInfo_t		dr;
	ll->xPos=data.x;
	ll->yPos=data.z;
	ahs->xPos=data.y;
	/*if(data.psi>=360.0&&!rolledOver){
		ahs->last_x[1][0]=data.psi;
		rolledOver=true;
	}
	else if(data.psi<=0.0&&!rolledOver){
		ahs->last_x[1][0]=data.psi;
		rolledOver=true;
	}*/
	/*if(data.psi<0)
	{
		printf("rolling %f %f %f %f\n",data.psi,ahs->last_x[1][0],ahs->last_x[2][0],ahs->last_x[3][0]);
		data.psi=360+data.psi;
		printf("rolling to %f %f %f %f\n",data.psi,ahs->last_x[1][0],ahs->last_x[2][0],ahs->last_x[3][0]);
	}
	if(data.psi>360)
	{
		printf("rolling %f %f %f %f\n",data.psi,ahs->last_x[1][0],ahs->last_x[2][0],ahs->last_x[3][0]);
		data.psi=data.psi-360;
		printf("rolling to %f %f %f %f\n",data.psi,ahs->last_x[1][0],ahs->last_x[2][0],ahs->last_x[3][0]);
	}*/
	float speed=velocity/velocity;
	/*if(speed>0&&(ll->last_x[2][0]>0.01||ll->last_x[2][0]<-0.01)&&(ll->last_x[3][0]>0.01||ll->last_x[3][0]<-0.01)){
		data.psi=(atan2(ll->last_x[2][0],-ll->last_x[3][0])* (180.0/3.141592653589793238463));
	}*/
	if(speed>0){
		data.psi=(atan2(velocity.x,-velocity.z)* (180.0/3.141592653589793238463));
	}
	//printf("flying %f %f %f %f %f\n",data.psi,ll->last_x[0][0],ll->last_x[1][0],ll->last_x[2][0],ll->last_x[3][0]);

	if(data.psi-ahs->last_x[1][0]>180.0||data.psi-ahs->last_x[1][0]<-180.0)
	{
		//printf("need to smooth %f %f %f %f %f\n",data.psi,ahs->last_x[1][0],ahs->last_x[2][0],ahs->last_x[3][0],(data.psi-ahs->last_x[1][0]));
		//if(ahs->last_x[1][0]>180)
		
		//else

		//ahs->last_x[1][0]=data.psi;
		//ahs->yPos=data.psi;
		//for(int i=0;i<40;i++)
		ahs->last_x[1][0]=data.psi;//180.0+ahs->last_x[1][0];
		//ahs->last_x[3][0]=ahs->last_x[1][0];
		//printf("became %f %f %f %f\n",data.psi,ahs->last_x[1][0],ahs->last_x[2][0],ahs->last_x[3][0]);
		
		//	ahs->frame();
		
	}
	/*else if((data.psi-ahs->getY()>180||data.psi+ahs->getY()>180)&&!rolledOver){
		ahs->last_x[1][0]=data.psi;
		rolledOver=true;
	}*/
	/*else if(data.psi>0.0&&data.psi<360.0){
		if(rolledOver){
			rolledOver=false;
			ahs->last_x[1][0]=data.psi;
		}
	}*/
	ahs->yPos=data.psi;
	rp->xPos=data.the;
	rp->yPos=data.phi;
	//if(id==2)
	ll->frame();
	ahs->frame();
	rp->frame();
	//printf("plane %d %f %f\n",id,data.psi,ahs->getY());
	dr.structSize = sizeof(dr);
	dr.x = ll->getX();//data.x;
	dr.y = ahs->getX()+yOffset;//data.y+yOffset;
	dr.z = ll->getY();//data.z;
	dr.pitch = rp->getX();//data.the;
	dr.heading = ahs->getY();//data.psi;
	dr.roll = rp->getY();//data.phi;

	clock_t timeNow = (clock()*60/CLOCKS_PER_SEC)%90+id*10.0;
	float touchDownSmoke=0.0;
	if(((double)clock())/CLOCKS_PER_SEC<(touchDownTime+3))
		touchDownSmoke=1.0;
	float thrust=timeNow+171.0;
	float gear=data.gear_deploy;

	if(velocity/velocity>0.1f){
		if(rpm<=100){
			//begin move
			startMoveTime=((double)clock())/(CLOCKS_PER_SEC*1.0f); 
			//printf(" 1 start %f %f %f\n", startMoveTime,rpm,velocity/velocity);
			rpm=110;
		}
		else if(rpm!=1500){
			double nowT=((double)clock())/(CLOCKS_PER_SEC*1.0f);
			if(nowT<(startMoveTime+1)){
				
        		double change=((nowT-startMoveTime)/1.0f);
				//printf(" 3 start %f %f %f\n", startMoveTime,change,velocity/velocity);
				rpm=101+1399*change;
				
			}
			else
				rpm=1500;
		}
	}else if(!data.engineoff){
		if(rpm==1500){
			startMoveTime=((double)clock())/(CLOCKS_PER_SEC*1.0f); 
			rpm=1400;
			//printf(" 2 start %f %f\n", startMoveTime,rpm);
		}
		else if(rpm!=100){
			double nowT=((double)clock())/(CLOCKS_PER_SEC*1.0f);
			if(nowT<(startMoveTime+1)){
				
        		double change=((nowT-startMoveTime)/1.0f);
				rpm=1499-1401*change;
				
			}
			else{
				inHover=false;
				rpm=100;
			}
			
		}
	}else{
		inHover=false;
		rpm=0;
	}

	thrust*=rpm;

	

    if(g_instance[0])
    {

		if(ref_style==0){	//cls_drefs	
			float tire[17] = {0,0,gear,gear*0.5,0,1.0,1.0,gear*useNavLights,useNavLights,0,0,0,touchDownSmoke,thrust,thrust,NULL};
			XPLMInstanceSetPosition(g_instance[0], &dr, tire);
		}
		else if(ref_style==1){//wt3_drefs
			float tire[19] = {0,0,gear,gear*0.5,0,1.0,1.0,gear*useNavLights,useNavLights,0,0,0,touchDownSmoke,thrust,thrust,thrust,thrust,NULL};
			XPLMInstanceSetPosition(g_instance[0], &dr, tire);
		}
		else{ //xmp_drefs
			float tire[19] = {0,0,gear,gear*0.5,0,1.0,1.0,gear*useNavLights,useNavLights,0,0,0,touchDownSmoke,(float)rpm,(float)rpm,thrust,thrust,0,NULL};
			XPLMInstanceSetPosition(g_instance[0], &dr, tire);
		}
        
    }
}


bool liveThread=false;
bool run=true;
//std::mutex g_ac_mutex[30];
void sendData(){
	JVM* jvmO=getJVM();
	if(jvmO->setIcaov){
		jvmO->setThreadData();
		PlaneData localData=jvmO->getPlaneData(-1,jvmO->plane_env);
		jvmO->live=localData.live;
	}
}
void do_model(){
	try{
		bool attached=false;
	JVM* jvmO=getJVM();
	while(!liveThread&&run){
		std::this_thread::sleep_for(std::chrono::seconds(1));
		
		if(jvmO->hasjvm&&!attached){
			jvmO->joinThread();
			attached=true;
		}
		if(attached)
			sendData();
	}
	/*JavaVMAttachArgs args;
	args.version = JNI_VERSION_1_6; // choose your JNI version
	args.name = NULL; // you might want to give the java thread a name
	args.group = NULL; // you might want to assign the java thread to a ThreadGroup
	jvm->AttachCurrentThread((void**)&myNewEnv, &args);*/
	if(jvmO->hasjvm&&!attached){
			jvmO->joinThread();
			attached=true;
	}
	printf("plane thread woke up\n");
	
	
	//printf("thread ready\n");
	while(liveThread&&run){
		sendData();
		jvmO->getThreadCommandData();
		for(int i=0;i<30;i++){
			//g_ac_mutex[i].lock();
			
			
			//printf("thread %d\n",i);
			aircraft[i].GetAircraftThreadData();
			//g_ac_mutex[i].unlock();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	
	//if(attached)
	{
		JVM* jvmO=getJVM();
		jvmO->jvm->DetachCurrentThread();
	}
	}catch (const std::exception& ex){

	}
	printf("plane thread stopped\n");
}
std::thread m_thread(do_model);

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
	
	//
	 
    for(int i=0;i<30;i++){
		aircraft[i].id=i+1;
	}
	liveThread=true;
	XPLMRegisterFlightLoopCallback(	AircraftLoopCallback,/* Callback */-1.0,	/* Interval */NULL);/* refcon not used. */

}
void stopPlanes(){
    //XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback0, NULL);
	printf("stopPlanes\n");
	if(liveThread){
		XPLMUnregisterFlightLoopCallback(AircraftLoopCallback, NULL);
		
	}
	liveThread=false;
	run=false;
	if(m_thread.joinable())
		m_thread.join();
    
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
		for(int i=0;i<30;i++){
			aircraft[i].GetAircraftData();
		}
		XPLMDebugString("AutoATC: started AI planes.\n");
       
        {
            XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"cjs/world_traffic/engine_rotation_angle1");
            XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"cjs/world_traffic/engine_rotation_angle2");
            XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"traf/.s");
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

//AircraftData userData;
int call=0;
float	AircraftLoopCallback(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon)
{
	
    
	// Get User Aircraft data

	/*
	for(int i=0;i<30;i++){
		aircraft[i].GetAircraftData();
		
	}
	*/
	for(int i=0;i<30;i++){
		//g_ac_mutex[i].lock();
		aircraft[i].PrepareAircraftData();
		aircraft[i].SetAircraftData();
		//g_ac_mutex[i].unlock();
	}
	soundSystem.update();
	//double nowlooptime=clock();
	//printf("flightloop %f\n",(nowlooptime-startlooptime));
	return -1;
}
