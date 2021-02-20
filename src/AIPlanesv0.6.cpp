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
#include <string.h>
#include <math.h>
//#include <cmath.h>
#include <stdio.h>
//#include "vec_opps.h"
#include "XPLMPlanes.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include <string>
#include "XPLMDisplay.h"
#include "XPLMScenery.h"
#include "XPLMInstance.h"
#include "XPLMGraphics.h"
//#include "XPLMPlugin.h"
 #include "XPLMMenus.h"
#include "jvm.h"
#include "Simulation.h"
 #include "aiplane.h"


  #include "AISound.h"
  #include "datarefs.h"
  #include "tcas.h"
 #include <thread>
//#include <mutex>
#include <chrono>
#define MSG_ADD_DATAREF 0x01000000           //  Add dataref to DRE message
#define PI 3.14159265
/*static XPLMDataRef		gPlaneX = NULL;
static XPLMDataRef		gPlaneY = NULL;
static XPLMDataRef		gPlaneZ = NULL;
static XPLMDataRef		gPlaneTheta = NULL;
static XPLMDataRef		gPlanePhi = NULL;
static XPLMDataRef		gPlanePsi = NULL;
static XPLMDataRef		gdaylight = NULL;
static XPLMDataRef		gOverRidePlanePosition = NULL;
static XPLMDataRef		gAGL = NULL;*/
                                
float	BeginAI();

							    
AircraftData::AircraftData(){
	//time=clock();
 }

Aircraft::Aircraft()
{
	ground_probe = XPLMCreateProbe(xplm_ProbeY);
	yOffset=5.0;//yOffsetp;
	airFrameIndex=-1;
	toLoadAirframe=false;
	//ll=new Simulation(0.01);
	ll=new Simulation(0.1);
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
int inLoading=0;
/*typedef struct {
	int acID;
	int pID;
} acModelDef;*/

static void loadedobject(XPLMObjectRef inObject, void *inRef){
	acModelDef acDef=*(acModelDef *)inRef;
	int id=acDef.acID;//*(int *)inRef;
	int modelPart=acDef.pID;//*(int *)modelPartRef;
	char debugStr[512];
	sprintf(debugStr,"loaded object C++id=%d ref=%d (%d)\n",id,inObject,inLoading);
	printf(debugStr);
#if defined(DEBUG_STRINGS)
	
	XPLMDebugString(debugStr);
	
#endif
	aircraft[id-1].setModelObject(inObject,modelPart);
	inLoading--;
	
	
}
int pid=25;
char found_path[1024];
 /*static void load_cb(const char * real_path, void * ref)
{

	XPLMObjectRef * dest = (XPLMObjectRef *) ref;
	if(*dest == NULL)
	{
		sprintf(found_path,"%s",real_path);
		printf("queued loading object %s\n",found_path);
	}
	else
		printf("skipped loading object %s\n",real_path);
}*/
void Aircraft::setModelObject(XPLMObjectRef inObject,int partID){
	#if defined(XP11)
	//XPLMCreateInstance(inObject, drefs);
	if(inObject)
	        {
				if(g_instance[partID])
					XPLMDestroyInstance(g_instance[partID]);
				if(ref_style==0)
					g_instance[partID] = XPLMCreateInstance(inObject, cls_drefs);
				else if(ref_style==1)
					g_instance[partID] = XPLMCreateInstance(inObject, wt3_drefs);
				else if(ref_style==3){
					g_instance[partID] = XPLMCreateInstance(inObject, acf_drefs);	
				}
				else if(ref_style==2)
					g_instance[partID] = XPLMCreateInstance(inObject, xmp_drefs);
            }
	#endif
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
		XPLMDebugString("AutoATC: Exception getting plane data");
		return;
	}
}
void Aircraft::SimulateAircraftThreadData(){
	data_mutex.lock();
	if(!thisData.live){				
		data_mutex.unlock();
		return;
	}
	ll->xPos=data.x;
	ll->yPos=data.z;
	ahs->xPos=data.y;

	float speed=velocity/velocity;

	if(speed>0.02){
		data.psi=(atan2(velocity.x,-velocity.z)* (180.0/3.141592653589793238463));
	}


	if(data.psi-ahs->last_x[1][0]>180.0||data.psi-ahs->last_x[1][0]<-180.0)
	{

		ahs->last_x[1][0]=data.psi;//180.0+ahs->last_x[1][0];

		
	}

	ahs->yPos=data.psi;
	rp->xPos=data.the;
	rp->yPos=data.phi;
	//if(id==2)
	ll->frame();
	ahs->frame();
	rp->frame();
	data_mutex.unlock();
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
		thisData.lat=newData.lat;
    	thisData.lon=newData.lon;
   		thisData.alt=newData.alt;   
    	thisData.gearDown=newData.gearDown;
    	thisData.throttle=newData.throttle;
    	thisData.the=newData.the;
		thisData.phi=newData.phi;
		thisData.psi=newData.psi;
    	thisData.timeStamp=newData.timeStamp;
		float timeNow = jvmO->getSysTime();//clock();
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
		XPLMDebugString("AutoATC: Exception getting plane data");
		return;
	}
}

void Aircraft::PrepareAircraftData()
{
   
    
	
	
	data_mutex.lock();
	JVM* jvmO=getJVM();

	if(!thisData.live){
				#if defined(XP11)
		        if(g_instance[0])
		        {
					printf("dropping object for id= %d\n",(id-1));
					for(int i=0;i<modelCount;i++){
						
						if(g_instance[i]!=NULL){
						XPLMInstanceRef kill=g_instance[i];
						g_instance[i]=NULL;
						XPLMDestroyInstance(kill);
						}
					}
					modelCount=1;
					airFrameIndex=-1;
					toLoadAirframe=false;
					//inLoading=false;
				}
				#endif
				
		data_mutex.unlock();
		return;
	}
	soundSystem.showActive();
	if(inLoading==0&&toLoadAirframe){
		visible=false;
		toLoadAirframe=false;
		char* af=jvmO->getModel(airFrameIndex);
		
#if defined(XP11)	
		
		if(g_instance[0]!=NULL)
	    {
#if defined(DEBUG_STRINGS)
			printf("dropping inloading object for load id= %d\n",id);
#endif
			for(int i=0;i<modelCount;i++){
					if(g_instance[i]!=NULL){
						XPLMInstanceRef kill=g_instance[i];
						g_instance[i]=NULL;
						XPLMDestroyInstance(kill);
					}
					
			}
					
		}
#endif
#if defined(DEBUG_STRINGS)
		XPLMDebugString("toLoadAirframe\n");
		sprintf(debugStr,"searching for id=%d afI=%d to %s sound=%d\n",id,airFrameIndex,af,soundIndex);
		XPLMDebugString(debugStr);
#endif
		
		//int *pid=&id;
		//inLoading=true;
		//acDef->acID=id;
		//acDef->pID=0;
		std::string afData (af); 
		std::size_t found = afData.find("?");
		if (found==std::string::npos){
			acModelDef *acDef=jvmO->getModelPart(airFrameIndex,0);
			acDef->acID=id;
			acDef->pID=0;
			modelCount=1;
			std::size_t foundV=afData.find("|");
			if (foundV==std::string::npos){
				acDef->partoffsets.x=acDef->partoffsets.y=acDef->partoffsets.z=0.0;
				inLoading++;
				XPLMLoadObjectAsync(af, loadedobject,acDef);
			}
			else{
				std::string pathS = afData.substr (0,foundV);
				inLoading++;
				XPLMLoadObjectAsync(pathS.c_str(), loadedobject,acDef);
				std::size_t foundV2 = afData.find("|",foundV+1);
				pathS = afData.substr (foundV+1,foundV2-foundV-1);
				char* end;
				acDef->partoffsets.x=strtod(pathS.c_str(),&end);
				foundV=foundV2;
				foundV2 = afData.find("|",foundV+1);
				pathS = afData.substr (foundV+1,foundV2-foundV-1);
				//char* end;
				acDef->partoffsets.y=strtod(pathS.c_str(),&end);
				foundV=foundV2;
				foundV2 = afData.find("|",foundV+1);
				pathS = afData.substr (foundV+1,foundV2-foundV-1);
				acDef->partoffsets.z=strtod(pathS.c_str(),&end);
				printf("got offsets %f %f %f\n",acDef->partoffsets.x,acDef->partoffsets.y,acDef->partoffsets.z);
			}	
		}
		else{
			std::string pathS = afData.substr (0,found);

    		std::size_t found2 = afData.find("?",found+1);
			acModelDef *acDef=jvmO->getModelPart(airFrameIndex,0);
			modelCount=1;
			acDef->acID=id;
			acDef->pID=0;
			std::size_t foundV=pathS.find("|");
			std::string pathSV = pathS.substr (0,foundV);
			
			if (foundV==std::string::npos){
				acDef->partoffsets.x=acDef->partoffsets.y=acDef->partoffsets.z=0.0;
				printf("to sub model load %s part %d\n",pathSV.c_str(),0);
				inLoading++;
				XPLMLoadObjectAsync(pathSV.c_str(), loadedobject,acDef);
				printf("got nill offsets %f %f %f\n",acDef->partoffsets.x,acDef->partoffsets.y,acDef->partoffsets.z);
			}
			else{
				
				printf("to sub model load %s part %d\n",pathSV.c_str(),0);
				inLoading++;
				XPLMLoadObjectAsync(pathSV.c_str(), loadedobject,acDef);
				std::size_t foundV2 = afData.find("|",foundV+1);
				pathS = afData.substr (foundV+1,foundV2-foundV-1);
				char* end;
				acDef->partoffsets.x=strtod(pathS.c_str(),&end);
				foundV=foundV2;
				foundV2 = afData.find("|",foundV+1);
				pathS = afData.substr (foundV+1,foundV2-foundV-1);
				//char* end;
				acDef->partoffsets.y=strtod(pathS.c_str(),&end);
				foundV=foundV2;
				foundV2 = afData.find("|",foundV+1);
				pathS = afData.substr (foundV+1,foundV2-foundV-1);
				acDef->partoffsets.z=strtod(pathS.c_str(),&end);
				printf("got offsets %f %f %f\n",acDef->partoffsets.x,acDef->partoffsets.y,acDef->partoffsets.z);
			}	
			
			//XPLMLoadObjectAsync(pathS.c_str(), loadedobject,acDef);
			while(found!=std::string::npos){

				pathS = afData.substr (found+1,found2-found-1);
				acDef=jvmO->getModelPart(airFrameIndex,modelCount);
				acDef->acID=id;
				acDef->pID=modelCount;
				std::size_t foundV=pathS.find("|");
				std::string pathSV = pathS.substr (0,foundV);
				
				if (foundV==std::string::npos){
					acDef->partoffsets.x=acDef->partoffsets.y=acDef->partoffsets.z=0.0;
					printf("to sub model load %s part %d\n",pathSV.c_str(),modelCount);
					inLoading++;
					XPLMLoadObjectAsync(pathSV.c_str(), loadedobject,acDef);
					printf("got nill offsets %f %f %f\n",acDef->partoffsets.x,acDef->partoffsets.y,acDef->partoffsets.z);
				}
				else{
					
					printf("to sub model load %s part %d\n",pathSV.c_str(),modelCount);
					inLoading++;
					XPLMLoadObjectAsync(pathSV.c_str(), loadedobject,acDef);
					pathSV=pathS;
					std::size_t foundV2 = pathSV.find("|",foundV+1);
					pathS = pathSV.substr (foundV+1,foundV2-foundV-1);
					char* end;
					acDef->partoffsets.x=strtod(pathS.c_str(),&end);
					foundV=foundV2;
					foundV2 = pathSV.find("|",foundV+1);
					pathS = pathSV.substr (foundV+1,foundV2-foundV-1);
					//char* end;
					acDef->partoffsets.y=strtod(pathS.c_str(),&end);
					foundV=foundV2;
					foundV2 = pathSV.find("|",foundV+1);
					pathS = pathSV.substr (foundV+1,foundV2-foundV-1);
					acDef->partoffsets.z=strtod(pathS.c_str(),&end);
					printf("got offsets %f %f %f\n",acDef->partoffsets.x,acDef->partoffsets.y,acDef->partoffsets.z);
				}
				//printf("to sub model load %s part %d\n",pathS.c_str(),acDef->pID);
				modelCount++;

				//XPLMLoadObjectAsync(pathS.c_str(), loadedobject,acDef);

				found=found2;
				if(found!=std::string::npos){
					found2 = afData.find("?",found+1);
					
				}
			}
    		//std::string pathS2 = afData.substr (found+1,found2);
			//printf("to load %s and %s\n",pathS.c_str(),pathS2.c_str());
		}
	}

	
    float timeNow = jvmO->getSysTime();//clock();
	float lastTime= lastData.time;
	double duration=((double)timeNow-(double)lastTime);// /CLOCKS_PER_SEC;
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
	float speed=velocity/velocity;

	double x=lastData.x+(vx*pComplete);
	double y=lastData.y+(vy*pComplete);
	double z=lastData.z+(vz*pComplete);
	//printf("Aircraft %d is %d and %f\n",id,thisData.live,y);	
	data.x = x;
	data.y = y;
	data.z = z;
	data.the=thisData.the;
    data.phi=thisData.phi;
		
	float setPHI=data.phi;//so we can unlock sooner
	int inAirframe=thisData.airframe;//so we can unlock sooner
	if(thisData.gearDown==3.0){
		data.inTransit=true;
		data.engineoff=false;
		//printf("%d in transit\n",id);
	}
	else if(thisData.gearDown==2.0){
		data.engineoff=true;
		if(speed==0.0f)
			data.psi=thisData.psi;
		data.inTransit=false;
	}
	else{
		data.engineoff=false;//false;
		data.inTransit=false;
	}
	if(!visible){
		
		v distanceV=v(x-ll->getX(),y-ahs->getX(),z-ll->getY());

		double deviation=distanceV/distanceV;
		if(data.engineoff){
			nextData.y-=50.0;
			lastData.y-=50.0;
			data.y-=50.0;//make parked aircraft rise from the grave :p
		}
		if((!data.engineoff&&deviation<100)||(data.engineoff&&deviation<5)){
			visible=true;
	#if defined(DEBUG_STRINGS)		
			printf("AUTOATC: made %d visible with %f\n",id,deviation);
	#endif
		}
		/*else{
			printf("AUTOATC: %d invisible with %f\n",id,deviation);
		}*/
	}
    data_mutex.unlock();
	XPLMProbeInfo_t outInfo;
	outInfo.structSize = sizeof(outInfo);
	//XPLMProbeResult ground= 
	double requestedAGL=1000.0;
	double targetAGL=1000.0;
	//XPLMProbeTerrainXYZ(ground_probe,data.x,data.y,data.z,&outInfo);
	double thisRequestedAGL=1000;
	if(!data.inTransit){
	 	XPLMProbeTerrainXYZ(ground_probe,data.x,data.y,data.z,&outInfo);
		requestedAGL=data.y-outInfo.locationY;
		targetAGL=ahs->getX()-outInfo.locationY;
	}/*else{
		if(thisRequestedAGL<100)
			printf("%d hit terrain\n");
		float now=jvmO->getSysTime();//((double)clock())/(CLOCKS_PER_SEC*1.0f);
		if(now>(data.updateAlt+30)){
			data.updateAlt=now;
			printf("%f above terrain\n",thisRequestedAGL);
		}
	}*/
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
	
		data.onGround=false;
		if(requestedAGL<18.0){
			data.onGround=true;
			data.y=outInfo.locationY;
			if(inTouchDown&&targetAGL<0.5){
				touchDownTime=jvmO->getSysTime();///CLOCKS_PER_SEC;
				soundSystem.land(id-1);
				inTouchDown=false;
			}
			data.the=-1.0;
    		data.phi=0.0;
			if(soundIndex==2){//Heli
				
				float now=jvmO->getSysTime();//((double)clock())/(CLOCKS_PER_SEC*1.0f);
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
			
    		data.phi=setPHI/3;
			if(soundIndex!=2)//no screech for helos
				inTouchDown=true;
			requestedAGL=((requestedAGL-18.0)*1.1);
			if(soundIndex==2){
				requestedAGL+=2;
				if(data.the>-1.0){
					data.the=-1.0;
					data.phi=setPHI/3;
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
		if((airFrameIndex==-1&&airFrameIndex!=inAirframe)||(inAirframe>0&&airFrameIndex!=inAirframe)){

			soundIndex=jvmO->getSound(inAirframe);
			icon=jvmO->getIcon(inAirframe);
			yOffset=jvmO->getOffset(inAirframe);

			ref_style=jvmO->getDrefStyle(inAirframe);
#if defined(DEBUG_STRINGS)
			sprintf(gBob_debstr3,"AutoATC switch Airframe from %d to %d. at %f style=%d\n",airFrameIndex,inAirframe,yOffset,ref_style);
        	XPLMDebugString(gBob_debstr3);
			printf(gBob_debstr3);
#endif
			airFrameIndex=inAirframe;
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
	TCASAPI* tcasAPI=getTCASAPI();
	if(!thisData.live){
		tcasAPI->SetData(id+1,0,0,0,0,0,0,0,-9999,0);
		return;
	}
		JVM* jvmO=getJVM();
    XPLMDrawInfo_t		dr;
	
	dr.structSize = sizeof(dr);
	if(visible){
		/*double xV=0;
		double yV=0;
		double zV=0;
		XPLMWorldToLocal(ll->getX(),ll->getY(),ahs->getX(),&xV,&yV,&zV);
		dr.x = xV;
		dr.y = yV+yOffset;//data.y+yOffset;
		dr.z = zV;*/
		dr.x = ll->getX();//data.x;
		dr.y = ahs->getX()+yOffset;//data.y+yOffset;
		dr.z = ll->getY();//data.z;

		
	}
	else
	{
		dr.x = 0.0;
		dr.y = -10000.0;
		dr.z = 0.0;
		tcasAPI->SetData(id+1,0,0,0,0,0,0,0,-9999,0);
	}
	
	dr.pitch = rp->getX();//data.the;
	dr.heading = ahs->getY();//data.psi;
	dr.roll = rp->getY();//data.phi;

	float timeNow = (clock()*60/CLOCKS_PER_SEC)%90+id*10.0;
	float touchDownSmoke=0.0;
	//if(((double)clock())/CLOCKS_PER_SEC<(touchDownTime+3))
	if(jvmO->getSysTime()<(touchDownTime+3))
		touchDownSmoke=1.0;
	float thrust=timeNow+171.0;
	float gear=data.gear_deploy;
	float landingLights=0.0;
	float flaps=0.0;
	if((!data.onGround&&!data.inTransit)||touchDownSmoke>0.0)
		landingLights=1.0;
	if(!data.inTransit&&!data.engineoff)	
		flaps=0.7;
	if(velocity/velocity>0.1f){
		if(rpm<=100){
			//begin move
			startMoveTime=jvmO->getSysTime();//((double)clock())/(CLOCKS_PER_SEC*1.0f); 
			//printf(" 1 start %f %f %f\n", startMoveTime,rpm,velocity/velocity);
			rpm=110;
		}
		else if(rpm!=1500){
			float nowT=jvmO->getSysTime();//((double)clock())/(CLOCKS_PER_SEC*1.0f);
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
			startMoveTime=jvmO->getSysTime();//((double)clock())/(CLOCKS_PER_SEC*1.0f); 
			rpm=1400;
			//printf(" 2 start %f %f\n", startMoveTime,rpm);
		}
		else if(rpm!=100){
			float nowT=jvmO->getSysTime();//((double)clock())/(CLOCKS_PER_SEC*1.0f);
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
	if(currentrpm<rpm)
		currentrpm++;
	else if(currentrpm>rpm)
		currentrpm--;
	thrust*=currentrpm;

#if defined(XP11)	
	for(int i=0;i<modelCount;i++)
    if(g_instance[i])
    {
		float thisdamage=0.0;
		if(damage!=NULL&&i==0){
			//float donedamage;
			
			/*XPLMGetDatavf(damage,&donedamage,id,1);
			if(donedamage>20)
				thisdamage=1.0;
			XPLMSetDatavi(acID,&id,id,1);
			XPLMSetDatavi(acAF,&airFrameIndex,id,1);
			XPLMSetDatavf(acX,&dr.x,id,1);
			XPLMSetDatavf(acY,&dr.y,id,1);
			XPLMSetDatavf(acZ,&dr.z,id,1);	*/

			g_my_idarray[id]=id;
			g_my_afarray[id]=airFrameIndex;
    		g_my_xarray[id]=dr.x;
    		g_my_yarray[id]=dr.y;
    		g_my_zarray[id]=dr.z;
			if(g_my_damagearray[id]>20)
				thisdamage=1.0;
    		
    
		}
		if(visible&&i==0)
			tcasAPI->SetData(id+1,1,(float)dr.x,(float)dr.y,(float)dr.z,(float)dr.heading,thisData.lat,thisData.lon,thisData.alt,icon);
		float engineon=!data.engineoff?1.0:0.0;
		if(ref_style==0){	//cls_drefs	
			float drefVals[19] = {0,0,gear,gear,0,1.0,1.0,gear*useNavLights,useNavLights,0,0,0,touchDownSmoke,thrust,thrust,thisdamage,(float)dr.y,NULL};
			
			XPLMInstanceSetPosition(g_instance[i], &dr, drefVals);
		}
		else if(ref_style==1){//wt3_drefs
			float drefVals[21] = {0,0,gear,gear,0,1.0,1.0,gear*useNavLights,useNavLights,0,0,0,touchDownSmoke,thrust,thrust,thrust,thrust,thisdamage,(float)dr.y,NULL};
			XPLMInstanceSetPosition(g_instance[i], &dr, drefVals);
		}
		else if(ref_style==3){
			float spin=((int)(thrust)%360);;
			float nowT=jvmO->getSysTime();//((double)clock())/(CLOCKS_PER_SEC*1.0f);
			
			if(nowT>(nextFlakTime)){
				startFlakTime=nowT+1+((float)(rand()%4)) / 3.0;
				nextFlakTime=nowT+10+(rand() % 10);
				flak=((float)(rand()%9)) / 10.0;
			}
			if(nowT>(startFlakTime)){
				flak=1.0;
			}
			
			
			
			//printf("damage %d=%f",id,donedamage);	
			acModelDef *acDef=jvmO->getModelPart(airFrameIndex,i);
			dr.x = ll->getX()+acDef->partoffsets.x;//data.x;
			dr.y = ahs->getX()+yOffset+acDef->partoffsets.y;//data.y+yOffset;
			dr.z = ll->getY()+acDef->partoffsets.z;
			
			float drefVals[10] = {(float)8.0,(float)8.0,spin,spin,flak,engineon,gear,thisdamage,NULL};
			XPLMInstanceSetPosition(g_instance[i], &dr, drefVals);
		}
		else{ //xmp_drefs
			//float drefVals[21] = {0,0,gear,gear*0.5f,0,1.0,1.0,gear*useNavLights,useNavLights,0,0,0,touchDownSmoke,(float)rpm,(float)rpm,thrust,thrust,0,thisdamage,(float)dr.y,NULL};
			float drefVals[20] = {engineon,engineon,engineon,gear,flaps,landingLights,thrust,thrust,thrust,thrust,(float)rpm,(float)rpm,(float)rpm,(float)rpm,(float)rpm,(float)rpm,0.0,thisdamage,(float)dr.y,NULL};
			XPLMInstanceSetPosition(g_instance[i], &dr, drefVals);
		}
     
    }
 #endif  
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
static void do_simulation(){
	while(!liveThread&&run){
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	printf("AutoATC: simulation thread woke up\n");
	char gBob_debstr2[128];
    sprintf(gBob_debstr2,"AutoATC: simulation thread woke up\n");
	while(liveThread&&run){
		auto start = std::chrono::high_resolution_clock::now();
		for(int i=0;i<30;i++){
			aircraft[i].SimulateAircraftThreadData();
		}
		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = finish - start;
		int diff=std::round(elapsed.count());
		//printf("%d\n",diff);
		if(diff<10)
			std::this_thread::sleep_for(std::chrono::milliseconds(10-diff));//100fps or less
	}
#if defined(DEBUG_STRINGS)
	printf("simulation thread stopped\n");
	sprintf(gBob_debstr2,"AutoATC: simulation thread stopped\n");
#endif
}
static void do_model(){
	try{
		bool attached=false;
	JVM* jvmO=getJVM();
	while(!liveThread&&run){
		std::this_thread::sleep_for(std::chrono::seconds(1));
		//std::this_thread::sleep_for(std::chrono::seconds(15));
		if(jvmO->loadLibraryFailed)
			run=false;
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
#if defined(DEBUG_STRINGS)
	printf("plane thread woke up\n");
	char gBob_debstr2[128];
    sprintf(gBob_debstr2,"plane thread woke up\n");

    XPLMDebugString(gBob_debstr2);
#endif
	int rolls=0;
	//printf("thread ready\n");
	while(liveThread&&run){
		auto start = std::chrono::high_resolution_clock::now();
		sendData();
		
		rolls++;
		if(rolls==10){
			jvmO->getThreadCommandData();
			jvmO->retriveLogData();
			rolls=0;
			
		}
		if(jvmO->fireTransmit){
				jvmO->doTransmit();
		}
		if(jvmO->fireNewFreq){
			jvmO->updateStndbyFreq();
		}	
		for(int i=0;i<30;i++){
			//g_ac_mutex[i].lock();
			
			
			//printf("thread %d\n",i);
			aircraft[i].GetAircraftThreadData();
			//printf("%d is index %d waiting for %d live =%d\n",i,aircraft[i].airFrameIndex,aircraft[i].thisData.airframe,aircraft[i].thisData.live);
			//g_ac_mutex[i].unlock();
		}
		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = finish - start;
		int diff=std::round(elapsed.count());
		//printf("%d\n",diff);
		if(diff<20)
		std::this_thread::sleep_for(std::chrono::milliseconds(20-diff));//100fps or less
		//std::this_thread::sleep_for(std::chrono::milliseconds(20));
		//std::this_thread::sleep_for(std::chrono::seconds(15));
	}
	
	//if(attached)
	{
		JVM* jvmO=getJVM();
		if(jvmO->hasjvm)
			jvmO->jvm->DetachCurrentThread();
	}
	}catch (const std::exception& ex){

	}
#if defined(DEBUG_STRINGS)	
	printf("AutoATC: plane thread stopped\n");
#endif
}
std::thread m_thread(&do_model);

std::thread m_simthread(&do_simulation);
void initPlanes(){

	

    /*gPlaneX = XPLMFindDataRef("sim/flightmodel/position/local_x");
	gPlaneY = XPLMFindDataRef("sim/flightmodel/position/local_y");
	gPlaneZ = XPLMFindDataRef("sim/flightmodel/position/local_z");
	gPlaneTheta = XPLMFindDataRef("sim/flightmodel/position/theta");
	gPlanePhi = XPLMFindDataRef("sim/flightmodel/position/phi");
	gPlanePsi = XPLMFindDataRef("sim/flightmodel/position/psi");
	gOverRidePlanePosition = XPLMFindDataRef("sim/operation/override/override_planepath");
	gAGL = XPLMFindDataRef("sim/flightmodel/position/y_agl");
	gdaylight = XPLMFindDataRef("sim/graphics/scenery/airport_lights_on");*/
	damage = XPLMFindDataRef("autoatc/aircraft/target_damage");
	acID = XPLMFindDataRef("autoatc/aircraft/id");
	acAF = XPLMFindDataRef("autoatc/aircraft/af");
	acX = XPLMFindDataRef("autoatc/aircraft/x");
	acY = XPLMFindDataRef("autoatc/aircraft/y");
	acZ = XPLMFindDataRef("autoatc/aircraft/z");
	sysTimeRef = XPLMFindDataRef("sim/time/total_running_time_sec");

	BeginAI();
	
	//
	 
    for(int i=0;i<30;i++){
		aircraft[i].id=i+1;
	}
	liveThread=true;


}
void stopPlanes(){
	printf("stopPlanes\n");
	soundSystem.stop();

	liveThread=false;
	run=false;
	if(m_thread.joinable())
		m_thread.join();
	
	char gBob_debstr2[128];
    sprintf(gBob_debstr2,"AutoATC: plane thread stopped\n");
    XPLMDebugString(gBob_debstr2);
    if(m_simthread.joinable())
		m_simthread.join();
	sprintf(gBob_debstr2,"AutoATC: simulation thread stopped\n");
    XPLMDebugString(gBob_debstr2);
}
bool loaded=false;

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
		
        
		
    }

   

	return 0;
}

//AircraftData userData;
int call=0;
//float	AircraftLoopCallback(float                inElapsedSinceLastCall, float                inElapsedTimeSinceLastFlightLoop,   int                  inCounter,    void *               inRefcon)
//runs on main XP flight loop, spend as little time in here as possible
void aircraftLoop()
{
	
    if(!liveThread)
		return;
	// Get User Aircraft data
	//printf("flight loop\n");
	for(int i=0;i<30;i++){
		aircraft[i].PrepareAircraftData();
		aircraft[i].SetAircraftData();
		//g_ac_mutex[i].unlock();
	}
	//char message[256];
	if(sound_on==NULL){
		sound_on = XPLMFindDataRef("sim/operation/sound/sound_on");
		sound_paused = XPLMFindDataRef("sim/time/paused");
		volume_master = XPLMFindDataRef("sim/operation/sound/master_volume_ratio");
		volume_eng = XPLMFindDataRef("sim/operation/sound/engine_volume_ratio");
		volume_ext = XPLMFindDataRef("sim/operation/sound/exterior_volume_ratio");
		volume_prop = XPLMFindDataRef("sim/operation/sound/prop_volume_ratio");
		volume_env = XPLMFindDataRef("sim/operation/sound/enviro_volume_ratio");
		
		view_mode = XPLMFindDataRef("sim/graphics/view/view_is_external");
		canopy_ratio = XPLMFindDataRef("sim/operation/sound/users_canopy_open_ratio");
		door_open_ratio = XPLMFindDataRef("sim/operation/sound/users_door_open_ratio");
	}
	float sound_vol=1.0;
	int enable=XPLMGetDatai(sound_on);
	int paused=XPLMGetDatai(sound_paused);
	float slider_master = XPLMGetDataf(volume_master);
	float slider_exterior= XPLMGetDataf(volume_ext);
	int viewisexternal = XPLMGetDatai(view_mode); // Is view external? (1 = yes)
	float canopyopen = XPLMGetDataf(canopy_ratio); // Canopy open ratio
	float closedspace_volume_scalar = 0.5; // Scalar for AI sounds in closed spaces (e.g. cockpit)
	static float doorarray [10] = { }; //Door open ratio array
	static float doorsum = 0.0; //Helper
	int dooropen = 0; //Is any door open (1 = yes)?
	

	//Get door array dref values
	XPLMGetDatavf(door_open_ratio,doorarray,0,9);
	
	//Loop through door array dataref and summarize all the door values
	doorsum = 0.0;
	for (int i = 0; i < 10; i++) {
			doorsum = doorsum + doorarray[i];
	}
	//If any door is open, change variable value
	if (doorsum >= 0.075) {
		dooropen = 1;
	} else {
		dooropen = 0;
	}
		
	// AI SOUND MIXER
	if( enable != 0 and paused != 1){
		if( viewisexternal == 0 and canopyopen == 0 and dooropen == 0){
			sound_vol = slider_master * slider_exterior * closedspace_volume_scalar;
		} else {
			//static float temp = slider_master * slider_exterior * (closedspace_volume_scalar + (0.33 * canopyopen) + (0.33 * viewisexternal) + (0.33 * dooropen));
			//sound_vol = fmin(fmax(temp,1.0), 0.0); //Ugly clamping implementation
			sound_vol = slider_master * slider_exterior;
		}
		// Debug messages in AISound.cpp
	} else {
		// Debug messages in AISound.cpp
		sound_vol = 0.0;
	}
	
	//sprintf(message,"AutoATC: External view (%i), Door/Canopy (%i/%f)\n",viewisexternal,dooropen,canopyopen);
	//XPLMDebugString(message);
	//printf(message);
	soundSystem.update(sound_vol);
	//return -1;
}
