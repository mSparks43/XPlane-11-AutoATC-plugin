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
 #include "XPLMGraphics.h"
#include "datarefs.h"
 #include <stdlib.h>
 #include <string>
#include <algorithm>
#include "jvm.h"

float drefV=1.0;
float getGearState(void * inRefcon) {
    return drefV;
}
void setGearState(void * inRefcon,float outvalue) {
    drefV=outvalue;
    //return 0;
}



//moved to datarefs.h
//#define ARRAY_DIM 25
int g_my_idarray[ARRAY_DIM] = { 0 };
int g_my_afarray[ARRAY_DIM] = { 0 };
int g_my_xarray[ARRAY_DIM] = { 0 };
int g_my_yarray[ARRAY_DIM] = { 0 };
int g_my_zarray[ARRAY_DIM] = { 0 };
int g_my_damagearray[ARRAY_DIM] = { 0 };
//static string_dref acarsinarray;
double g_objectArray[30*8] = { 0.0 };
double g_incomingObjectArray[30*9] = { 0.0 };
static int_dref receivedAcars;
static int_dref onlineAcars;

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
std::vector<std::vector<std::string>> drefsDefs;
std::vector<std::vector<float>> drefsValues;
static int getautoatcDataRefvf(void * ref, float * out_values, int in_offset, int in_max)
{
	int n, r;
	if(out_values == NULL)
        return 2;
	

	return 2;
}
std::vector<float> getDrefValues(int aircraftID){
	std::vector<std::string> strings=drefsDefs[aircraftID-1];
	std::vector<float> values=drefsValues[aircraftID-1];
	if(values.size()<strings.size())
	{
		int count=strings.size()-values.size();
		for(int n=0;n<count;n++)
			values.push_back(0.0);
	}
	return values;
	 
	//const char ** rV=c_strs.data();
	//return rV;

}
static void setautoatcDataRefvf(void * ref, float * in_values, int in_offset, int in_max)
{
	long ac_id=(long)ref;
	printf("set drefs for %d\n",ac_id);
	std::vector<float> theseValues;
	for(int n = 0; n < (in_max); ++n){
        printf("Set %d %f\n",n+in_offset, in_values[n]);
		theseValues.push_back(in_values[n]);
	}
	if(ac_id+1>drefsValues.size()){
		drefsValues.push_back(theseValues);
	}
	else{
		drefsValues[ac_id]=theseValues;
	}

}

static int getautoatcDatavf(void * ref, float * out_values, int in_offset, int in_max)
{
	int n, r;
	if(out_values == NULL)
        return 30*9;
	r = 30*9 - in_offset;
	
    if(r > in_max) r = in_max;

	// Now copy the actual items from our array to the returned memory.
    for(n = 0; n < r; ++n) 
		out_values[n] = g_incomingObjectArray[n+in_offset]; 
		
	return r; 	
}

static int getobjectDatavf(void * ref, float * out_values, int in_offset, int in_max)
{
	int n, r;
	if(out_values == NULL)
        return 2;
	r = ARRAY_DIM - in_offset;
	
    if(r > in_max) r = in_max;

	// Now copy the actual items from our array to the returned memory.
    for(n = 0; n < r; ++n) out_values[n] = 0.0f; return r; 	
}

static void setobjectDatavf(void * ref, float * in_values, int in_offset, int in_max)
{
	//printf("Begin objectData set\n");
	int n, r; // Calculate the number of items to copy in. This is the lesser of the number // the caller writes and the end of our array. 
	// Copy the actual data.
    for(n = 0; n < (in_max); ++n){
        //printf("Set %d %f\n",n+in_offset, in_values[n]);
		g_objectArray[n+in_offset]=in_values[n];
	}
	//printf("End objectData set\n");
	//printf("Set %d to %d\n",in_offset, in_max);
}
//incoming json string array of datarefs for an aircraft


void setXTLuaPlanedatarefs(void * refA, void * in_values, int in_offset, int in_max)
{
	int new_length = in_offset + in_max;
	const char * source = (const char *) in_values;

	std::string	string_data;
	string_data.resize(new_length);
	for(int i = 0; i < in_max; ++i)
		string_data[i + in_offset] = source[i];
	json jdatarefs =json::parse(string_data);
	int ac_id=jdatarefs["id"].get<int>();
	std::vector<std::string> drefs=jdatarefs["datarefs"].get<std::vector<std::string>>();
	//printf("got string datarefs def:%s\n",string_data.c_str());
	//printf("first is:%s\n",drefs[0].c_str());
	drefsDefs.push_back(drefs);
}
std::vector<std::string> getDrefNames(int aircraftID){
	std::vector<std::string> strings=drefsDefs[aircraftID-1];
	return strings;
	 
	//const char ** rV=c_strs.data();
	//return rV;

}
void setXTLuaPlanedata(int id,PlaneData inVal)
{
	int startIndex=(id-1)*9;
	if(inVal.live==false){
		g_incomingObjectArray[startIndex]=-1;
		return;
	}
	g_incomingObjectArray[startIndex]=inVal.airframe;
	g_incomingObjectArray[startIndex+1]=inVal.lat;
    g_incomingObjectArray[startIndex+2]=inVal.lon;
    g_incomingObjectArray[startIndex+3]=inVal.alt;
    g_incomingObjectArray[startIndex+4]=inVal.the;
    g_incomingObjectArray[startIndex+5]=inVal.phi;
    g_incomingObjectArray[startIndex+6]=inVal.psi;
	g_incomingObjectArray[startIndex+7]=inVal.state;
	g_incomingObjectArray[startIndex+8]=inVal.agl;
}
PlaneData getXTLuaPlanedata(int id){
	PlaneData retVal;
	
	int startIndex=(id-1)*8;
	int airframe=g_objectArray[startIndex];
	if(airframe<=0.0){
		retVal.live=false;
		return retVal;
	}
	JVM* jvmO=getJVM();
	//printf("getting getXTLuaPlanedata for %d\n",id);
	//TODO, move this to setobjectDatavf
	retVal.live=true;
	retVal.airframe=(int)g_objectArray[startIndex];
	retVal.lat=g_objectArray[startIndex+1];
    retVal.lon=g_objectArray[startIndex+2];
    retVal.alt=g_objectArray[startIndex+3];
	XPLMWorldToLocal(retVal.lat,retVal.lon,retVal.alt,&retVal.x,&retVal.y,&retVal.z);
    retVal.the=(float)g_objectArray[startIndex+4];
    retVal.phi=(float)g_objectArray[startIndex+5];
    retVal.psi=(float)g_objectArray[startIndex+6];
	retVal.agl=(float)g_objectArray[startIndex+7];
	//printf("getting getXTLuaPlanedata for %d is %f\n",id,retVal.agl);
	retVal.timeStamp=jvmO->getSysTime();
	return retVal;
}
static int getvb(void * refA, void * out_values, int in_offset, int in_max)
{
    char * destination = (char *) out_values;
	string_dref * ref =(string_dref *)refA;
	if(out_values == NULL)
		return (int)ref->string_data.size();
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

	//printf("AutoATC:set string to ((%s))\n",ref->string_data.c_str());	

}
static void setsayvb(void * refA, void * in_values, int in_offset, int in_max)
{
    const char * source = (const char *) in_values;
	int new_length = in_offset + in_max;

	string_dref * ref =(string_dref *)refA;
	ref->string_data.resize(new_length);
	for(int i = 0; i < in_max; ++i)
		ref->string_data[i + in_offset] = source[i];
	//char command[1024]={0};
	//printf("AutoATC:set string to ((%s))\n",ref->string_data.c_str());
	//sprintf(command,"doCommand:sendAcars:%s",ref->string_data.c_str());
    //sprintf(acarsoutdata,"doCommand:sendAcars:%s",acarsoutdata);
    //printf("You say = %s\n",ref->string_data.c_str());
	JVM *jvmO = getJVM();
	jvmO->showSayWindow();	
    //

}
static void setacarsvb(void * refA, void * in_values, int in_offset, int in_max)
{
    const char * source = (const char *) in_values;
	int new_length = in_offset + in_max;

	string_dref * ref =(string_dref *)refA;
	ref->string_data.resize(new_length);
	for(int i = 0; i < in_max; ++i)
		ref->string_data[i + in_offset] = source[i];
	//char command[1024]={0};
	//printf("AutoATC:set string to ((%s))\n",ref->string_data.c_str());
	//sprintf(command,"doCommand:sendAcars:%s",ref->string_data.c_str());
	std::string command="doCommand:sendAcars:"+ref->string_data;
    //sprintf(acarsoutdata,"doCommand:sendAcars:%s",acarsoutdata);
    //printf("SEND ACARS = %s\n",command);
	JVM *jvmO = getJVM();
	jvmO->getData(command.c_str());	
    //

}
static void setcduvb(void * refA, void * in_values, int in_offset, int in_max)
{
    const char * source = (const char *) in_values;
	int new_length = in_offset + in_max;

	string_dref * ref =(string_dref *)refA;
	ref->string_data.resize(new_length);
	bool newData=false;
	for(int i = 0; i < in_max; ++i){
		if(ref->string_data[i + in_offset] != source[i])
			newData=true;
		ref->string_data[i + in_offset] = source[i];
	}
	if(newData){
		//char command[2048]={0};
		//printf("AutoATC:set string to ((%s))\n",ref->string_data.c_str());
		//sprintf(command,"doCommand:CDU:%s",ref->string_data.c_str());
		//sprintf(acarsoutdata,"doCommand:sendAcars:%s",acarsoutdata);
		//printf("SEND CDU = %s\n",command);
		std::string command="doCommand:CDU:"+ref->string_data;

		JVM *jvmO = getJVM();
		jvmO->getData(command.c_str());	
	}
    //

}
std::string getAcarsOut(){
	return acarsoutarray.string_data;
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
static int	get_logPage(void * refA)
{
	JVM *jvmO = getJVM();
	return jvmO->logPage;
}

static void	set_logPage(void * refA, int val)
{
	JVM *jvmO = getJVM();
	jvmO->logPage=val;
}
void registerDatarefs(){
	const char * cls_drefs[15]={ "traf/mw_def",
	"traf/nw_def",
	"traf/lgear",
	"traf/flaps",
	"traf/nw_rot",
	"traf/bcn",
	"traf/strobe"
	,"traf/land",
	"traf/nav",
	"traf/taxi",
	"traf/sb",
	"traf/vint",
	"traf/touch_down",
	"traf/vint_circle",
	"traf/vint_circle2"};
	const char * acf_drefs[7]={ "autoatc/engine/POINT_tacrad0",
	"autoatc/engine/POINT_tacrad1",
	"autoatc/engine/POINT_prop_ang_deg0",
	"autoatc/engine/POINT_prop_ang_deg1",
	"autoatc/flak",
	"autoatc/engineon",
	"autoatc/altitude"};
	const char * wt3_drefs[17]={ "traf/mw_def",
	"traf/nw_def",
	"cjs/world_traffic/main_gear_retraction_ratio",
	"cjs/world_traffic/nose_gear_retraction_ratio",
	"traf/nw_rot",
	"cjs/world_traffic/beacon_lights_on",
	"cjs/world_traffic/strobe_lights_on",
	"cjs/world_traffic/wing_landing_lights_on",
	"cjs/world_traffic/nav_lights_on",
	"cjs/world_traffic/taxi_lights_on",
	"traf/sb"
	,"traf/vint",
	"cjs/world_traffic/touch_down",
	"cjs/world_traffic/engine_rpm1",
	"cjs/world_traffic/engine_rpm2",
	"cjs/world_traffic/engine_rotation_angle1",
	"cjs/world_traffic/engine_rotation_angle2" };
	const char * xmp_drefs[17]={ "libxplanemp/controls/beacon_lites_on",
	"libxplanemp/controls/nav_lites_on",
	"libxplanemp/controls/strobe_lites_on",
	"libxplanemp/controls/gear_ratio",
	"libxplanemp/controls/flap_ratio",
	"libxplanemp/controls/landing_lites_on",
	"libxplanemp/engines/engine_rotation_angle_deg1",
	"libxplanemp/engines/engine_rotation_angle_deg2",
	"libxplanemp/engines/engine_rotation_angle_deg3",
	"libxplanemp/engines/engine_rotation_angle_deg4",
	"libxplanemp/engines/engine_rotation_speed_rad_sec1",
	"libxplanemp/engines/engine_rotation_speed_rad_sec2",
	"libxplanemp/engines/engine_rotation_speed_rpm1",
	"libxplanemp/engines/engine_rotation_speed_rpm2",
	"libxplanemp/engines/engine_rotation_speed_rpm3",
	"libxplanemp/engines/engine_rotation_speed_rpm4",
	"libxplanemp/controls/speed_brake_ratio"};
	for(int i=0;i<15;i++){

		XPLMRegisterDataAccessor(cls_drefs[i], xplmType_Float, 0, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	}
	for(int i=0;i<7;i++){

		XPLMRegisterDataAccessor(acf_drefs[i], xplmType_Float, 0, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	}
	for(int i=0;i<17;i++){

		XPLMRegisterDataAccessor(wt3_drefs[i], xplmType_Float, 0, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	}
	for(int i=0;i<17;i++){

		XPLMRegisterDataAccessor(xmp_drefs[i], xplmType_Float, 0, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL);
	}
	XPLMRegisterDataAccessor("autoatc/aircraft/id", xplmType_IntArray, 0, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    getvi, setvi,getvf, setvf, 
    NULL, NULL,g_my_idarray, g_my_idarray);//create it
	XPLMRegisterDataAccessor("autoatc/aircraft/af", xplmType_IntArray, 0, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    getvi, setvi,getvf, setvf, 
    NULL, NULL,g_my_afarray, g_my_afarray);//create it
	XPLMRegisterDataAccessor("autoatc/aircraft/x", xplmType_FloatArray, 0, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    NULL, NULL,getvf, setvf, 
    NULL, NULL,g_my_xarray, g_my_xarray);//create it
	XPLMRegisterDataAccessor("autoatc/aircraft/y", xplmType_FloatArray, 0, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    NULL, NULL,getvf, setvf, 
    NULL, NULL,g_my_yarray, g_my_yarray);//create it

	XPLMRegisterDataAccessor("autoatc/aircraft/z", xplmType_FloatArray, 0, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    NULL, NULL,getvf, setvf, 
    NULL, NULL,g_my_zarray, g_my_zarray);//create it

	XPLMRegisterDataAccessor("autoatc/aircraft/target_damage", xplmType_FloatArray, 0, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    NULL, NULL,getvf, setvf, 
    NULL, NULL,g_my_damagearray, g_my_damagearray);//create it
	XPLMRegisterDataAccessor("autoatc/aircraft/data", xplmType_FloatArray, true, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    NULL, NULL,getobjectDatavf, setobjectDatavf, 
    NULL, NULL,NULL, NULL);//create it
	XPLMRegisterDataAccessor("autoatc/aircraft/incomingdata", xplmType_FloatArray, false, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    NULL, NULL,getautoatcDatavf, NULL, 
    NULL, NULL,NULL, NULL);//create it
	char debugStr[512];
	for(int i=0;i<30;i++){
		
		sprintf(debugStr,"autoatc/aircraft/setdrefs/%d",i);
		XPLMRegisterDataAccessor(debugStr, xplmType_FloatArray, true, 
		NULL,NULL,  NULL, 
		NULL, NULL, NULL, 
		NULL, NULL,getautoatcDataRefvf, setautoatcDataRefvf, 
		NULL, NULL,(void *)i, (void *)i);//create it
	}
	XPLMRegisterDataAccessor("autoatc/definedatarefs", xplmType_Data, true, 
    NULL,NULL,  NULL, 
    NULL, NULL, NULL, 
    NULL, NULL,NULL, NULL, 
    NULL, setXTLuaPlanedatarefs,NULL, NULL);//create it
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
						getvb, setacarsvb,
						&acarsoutarray, &acarsoutarray);
	XPLMRegisterDataAccessor("autoatc/yousay", xplmType_Data, true,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						getvb, setsayvb,
						&sayarray, &sayarray);					
	XPLMRegisterDataAccessor("autoatc/cdu", xplmType_Data, true,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						getvb, setcduvb,
						&cduoutarray, &cduoutarray);
	XPLMRegisterDataAccessor("autoatc/acars/received", xplmType_Int, true,
						geti, seti,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						&receivedAcars, &receivedAcars);
	XPLMRegisterDataAccessor("autoatc/acars/online", xplmType_Int, true,
						geti, seti,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						&onlineAcars, &onlineAcars);
						
	XPLMRegisterDataAccessor("autoatc/logpage", xplmType_Int, true,
						get_logPage, set_logPage,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL,
						NULL, NULL);																
	
}
