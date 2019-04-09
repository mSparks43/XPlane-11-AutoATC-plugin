 #include <jni.h>
#include "jvm.h"
#include "xplane.h"
 #include "XPLMPlugin.h"
 #include "XPLMDisplay.h"
 #include "XPLMGraphics.h"
 #include "XPLMMenus.h"
 #include "XPLMProcessing.h"
 #include "XPLMDataAccess.h"
 #include "XPLMMenus.h"
 #include "XPLMUtilities.h"
 #include "XPWidgets.h"
 #include "XPStandardWidgets.h"
 #include "XPLMScenery.h"
 #include <string.h>
 #include <stdio.h>
  #include <math.h>
  #include "Settings.h"
  #include "vec_opps.h"
  #include "aiplane.h"
  #include <stdlib.h>
 #include <vector>
 #include <ctype.h>//isspace
 #if defined(__linux__) || defined(__APPLE__)
 #include <dlfcn.h>
 #include <stdlib.h>
 #endif
using namespace std;
 const char* plugin_version = "About:0.8.6.5b7";
char gBob_debstr2[128];
char xp_path[512];
char* CONFIG_FILE_DEFAULT_AIRFRAMES ="Resources/plugins/java/airframes_860.txt";
 bool file_exists (const std::string& name);
 void				draw_atc_text(XPLMWindowID in_window_id, void * in_refcon);
void				draw_about_text(XPLMWindowID in_window_id, void * in_refcon);
void	            draw_jvm_text(XPLMWindowID in_window_id, void * in_refcon);
int					dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon) { return 0; }
XPLMCursorStatus	dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void * in_refcon) { return xplm_CursorDefault; }
int					dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon) { return 0; }
void				dummy_key_handler(XPLMWindowID in_window_id, char key, XPLMKeyFlags flags, char virtual_key, void * in_refcon, int losing_focus) { }
//bool hot=false;
bool enabledATCPro;
void menu_handler(void *, void *);

 XPLMDataRef  transponder_codeRef = NULL;
 XPLMDataRef  latitudeRef = NULL;
 XPLMDataRef  longitudeRef = NULL;
 XPLMDataRef  altitudeRef = NULL;
 XPLMDataRef  battery_onRef = NULL;

XPLMDataRef  iasRef = NULL;
XPLMDataRef gyroHeadingRef = NULL;
XPLMDataRef altPressureRef = NULL;
XPLMDataRef  mslPressureRef = NULL;
XPLMDataRef fpmRef = NULL;
XPLMDataRef  pitchRef = NULL;
XPLMDataRef  rollRef = NULL;
 //XPLMDataRef  com1_freq_hzRef = NULL;
 //XPLMDataRef  com1_stdby_freq_hz = NULL;
 //const char* CONFIG_FILE ="Resources/plugins/java/jvmsettings.txt";
std::vector<AirframeDef> airframeDefs;
AirframeDef::AirframeDef(void){
    
}
void AirframeDef::setData(std::string inLine){
    std::size_t found = inLine.find(",");
    std::string pathS = inLine.substr (0,found);
    std::size_t found2 = inLine.find(",",found+1);
    std::string offsetS = inLine.substr (found+1,found2);
    std::size_t found3 = inLine.find(",",found2+1);
    std::string soundS = inLine.substr (found2+1,found3);
    std::string drefStyleS = inLine.substr (found3+1);

    //strcpy(path,pathS.c_str());
   // sprintf (path, "%s", pathS.c_str());
    sprintf (path, "%s%s", xp_path, pathS.c_str());
    char* end;
    //
    yOffset=strtod(offsetS.c_str(),&end);
    soundIndex=strtol(soundS.c_str(),&end,10);
    drefStyle=strtol(drefStyleS.c_str(),&end,10);
    /* std::size_t foundwt3 = inLine.find("WorldTraffic");

     if(foundwt3==std::string::npos)
        yOffset=0;
     else
        yOffset=2.0;
    //sprintf (path, "%s", inLine.c_str());*/
}
char* AirframeDef::getPath(void){
    return path;
}
double AirframeDef::getOffset(void){
    return yOffset;
}
int AirframeDef::getSound(void){
    return soundIndex;
}
int AirframeDef::getDrefStyle(void){
    return drefStyle;
}
JVM::JVM(void)
{

}
void JVM::deactivateJVM(void){
    if(hasjvm){
        stop();
        destroyMenu(); 
    }   
    hasjvm=false;  
}
void JVM::activateJVM(void){
    destroyMenu();
    try{
    
    hasjvm=false;
    if(!loadedLibrary){
#if defined(__linux__)
     sprintf(gBob_debstr2,"AutoATC:Loading jvm dll '%s' \n", lin);
     XPLMDebugString(gBob_debstr2);
     libnativehelper = dlopen(lin, RTLD_NOW);//"libjvm.so"
        if (libnativehelper==NULL) {
            hasjvm=false;
            dlerror();
            popupNoJVM();
             createMenu();
            return;
        }
        else{
            JNI_CreateJavaVM = (JNI_CreateJavaVM_t) dlsym(libnativehelper, "JNI_CreateJavaVM");
            JNI_GetCreatedJavaVMs = (JNI_GetCreatedJavaVMs_t) dlsym(libnativehelper, "JNI_GetCreatedJavaVMs");
            //JNI_AttachCurrentThread =(JNI_AttachCurrentThread_t)dlsym(libnativehelper, "AttachCurrentThread");
        }
    sprintf(gBob_debstr2,"AutoATC:Successfully loaded the jvm dll\n");
    XPLMDebugString(gBob_debstr2);
 #elif defined(_WIN64)
  sprintf(gBob_debstr2,"AutoATC:Loading jvm dll '%s' \n", win);
     XPLMDebugString(gBob_debstr2);
     SetErrorMode(0); 
    libnativehelper = LoadLibraryA(win); //"jvm.dll"
    if (!libnativehelper) {
            hasjvm=false;
            popupNoJVM();
             createMenu();
            return;
     }
    else{
        sprintf(gBob_debstr2,"AutoATC:Successfully loaded the jvm dll\n");
        JNI_CreateJavaVM = (JNI_CreateJavaVM_t) GetProcAddress(libnativehelper, "JNI_CreateJavaVM");
    }
    sprintf(gBob_debstr2,"AutoATC:found CreateJavaVM\n");
    XPLMDebugString(gBob_debstr2);
#elif defined(__APPLE__)
    libnativehelper = dlopen(mac, RTLD_NOW);//"/usr/local/jre/lib/server/libjvm.dylib"
    if (!libnativehelper) {
            hasjvm=false;
            dlerror();
            popupNoJVM();
             createMenu();
            return;
        }
        else
        JNI_CreateJavaVM = (JNI_CreateJavaVM_t) dlsym(libnativehelper, "JNI_CreateJavaVM");
#endif
     
        JavaVMInitArgs vm_args;                        // Initialization arguments
       JavaVMOption* options = new JavaVMOption[1];   // JVM invocation options
       options[0].optionString = "-Djava.class.path=Resources/plugins/java/AutoATCPlugin.jar";   // where to find java .class
       vm_args.version = JNI_VERSION_1_6;             // minimum Java version
       vm_args.nOptions = 1;                          // number of options
       vm_args.options = options;
       vm_args.ignoreUnrecognized = false;     // invalid options make the JVM init fail
           //=============== load and initialize Java VM and JNI interface =============
       jint rc = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);  // YES !!
       delete options;    // we then no longer need the initialisation options. 
      
       jint ver = env->GetVersion();
       sprintf(gBob_debstr2,"Java Version %d.%d \n", ((ver>>16)&0x0f),(ver&0x0f));
       XPLMDebugString(gBob_debstr2);
       
       commandsClass = env->FindClass("jni/Commands");  // try to find the class
    if(commandsClass == NULL) {
        sprintf(gBob_debstr2,"AutoATC ERROR: Commands not found !\n");
        XPLMDebugString(gBob_debstr2);
    }
    else {                                  // if class found, continue
        sprintf(gBob_debstr2,"AutoATC Commands found\n");
        XPLMDebugString(gBob_debstr2);
        broadcastMethod = env->GetStaticMethodID(commandsClass, "broadcast", "()V");  // find method
        setDataMethod = env->GetStaticMethodID(commandsClass, "setData", "([F)V");  // find method
       
         if(setDataMethod == NULL){
            XPLMDebugString("ERROR: method void setData(float[]) not found !\n");
            return;
         }
        if(broadcastMethod == NULL){
            XPLMDebugString("ERROR: method void broadcast() not found !\n");
            return;
        }
       getPlaneDataMethod = env->GetStaticMethodID(commandsClass, "getPlaneData", "(I)[D");  // find method
        if(getPlaneDataMethod == NULL){
            XPLMDebugString("ERROR: method void getPlaneDataMethod() not found !\n");
            return;
        }
        midToString=env->GetStaticMethodID(commandsClass, "getData", "(Ljava/lang/String;)Ljava/lang/String;");  // find method
        if(midToString == NULL){
            XPLMDebugString("ERROR: method void getData() not found !\n");
            return;
        }
        commandString=env->GetStaticMethodID(commandsClass, "getCommandData", "()Ljava/lang/String;");  // find method
        if(commandString == NULL){
            XPLMDebugString("ERROR: method void getCommandData() not found !\n");
            return;
        }
        getStndbyMethod=env->GetStaticMethodID(commandsClass, "getStndbyFreq", "(I)I");  // find method
        if(getStndbyMethod == NULL){
            XPLMDebugString("ERROR: method int getStndbyFreq() not found !\n");
            return;
        }
        printf("AutoATC active !\n");
         hasjvm=true;
    }
    loadedLibrary=true;
    }
    else{
        printf("AutoATC active !\n");
         hasjvm=true;
    }
    }
catch(...){
		printf("Exception during JVM test\n");
		return;
	}  
    createMenu();
}


char *JVM::trim (char * s)
{
  /* Initialize start, end pointers */
  char *s1 = s, *s2 = &s[strlen (s) - 1];

  /* Trim and delimit right side */
  while ( (isspace (*s2)) && (s2 >= s1) )
    s2--;
  *(s2+1) = '\0';

  /* Trim left side */
  while ( (isspace (*s1)) && (s1 < s2) )
    s1++;

  /* Copy finished string */
  strcpy (s, s1);
  return s;
}
void JVM::testExistingJVM (){
    jsize nVMs;
    JNIEnv *local_env;
    JNI_GetCreatedJavaVMs(NULL, 0, &nVMs); // 1. just get the required array length
    JavaVM** jvms = new JavaVM*[nVMs];
    JNI_GetCreatedJavaVMs(jvms, nVMs, &nVMs);
    if(nVMs==1){
        jvms[0]->AttachCurrentThread((void**)&local_env, NULL);
         if(local_env == NULL)
            printf("failed to attached \n");
            else{
                jclass apiClass = local_env->FindClass("api/XPJVMAPI");  // try to find the class
                if(apiClass != NULL)
                    printf("searched and found XPJVMAPI \n");
                else
                    printf("searched and NOT found XPJVMAPI \n");
            }
    }
}
void JVM::init_parameters ()
{

    strncpy (win, "jvm.dll", MAXLEN);
    strncpy (lin, "libjvm.so", MAXLEN);
    strncpy (mac, "/usr/local/jre/lib/server/libjvm.dylib", MAXLEN);
    airframeDefs.clear();
    XPLMGetSystemPath(xp_path);
    transponder_codeRef = XPLMFindDataRef("sim/cockpit/radios/transponder_code");
    latitudeRef = XPLMFindDataRef("sim/flightmodel/position/latitude");
    longitudeRef = XPLMFindDataRef("sim/flightmodel/position/longitude");
    altitudeRef = XPLMFindDataRef ("sim/flightmodel/position/elevation");
    battery_onRef = XPLMFindDataRef ("sim/cockpit/electrical/battery_on");
    com1_freq_hzRef = XPLMFindDataRef ("sim/cockpit/radios/com1_freq_hz");
    com1_stdby_freq_hz = XPLMFindDataRef ("sim/cockpit/radios/com1_stdby_freq_hz");
    iasRef = XPLMFindDataRef ("sim/cockpit2/gauges/indicators/airspeed_kts_pilot");
    gyroHeadingRef = XPLMFindDataRef ("sim/cockpit2/gauges/indicators/heading_vacuum_deg_mag_pilot");;
    altPressureRef = XPLMFindDataRef ("sim/cockpit2/gauges/indicators/altitude_ft_pilot");
    mslPressureRef = XPLMFindDataRef ("sim/cockpit2/gauges/actuators/barometer_setting_in_hg_pilot");
    fpmRef = XPLMFindDataRef ("sim/cockpit2/gauges/indicators/vvi_fpm_pilot");
    pitchRef = XPLMFindDataRef ("sim/cockpit2/gauges/indicators/pitch_vacuum_deg_pilot");
    rollRef = XPLMFindDataRef ("sim/cockpit2/gauges/indicators/roll_vacuum_deg_pilot");
    standbyAirframe=AirframeDef();
    standbyAirframe.setData("Resources/default scenery/airport scenery/Aircraft/Heavy_Metal/747United.obj,0.0,0,0");
}
void JVM::init_parameters (char * jvmfilename)
{
 // struct file_parameters * lparms=&parms;
  strncpy (win,jvmfilename, MAXLEN);
  strncpy (lin,jvmfilename, MAXLEN);
  strncpy (mac,jvmfilename, MAXLEN);
  //airframeDefs.clear();
  XPLMGetSystemPath(xp_path);
}

void JVM::parse_config (char * filename)
{
   // struct file_parameters * lparms=&parms;
  char *s, buff[256];
  FILE *fp = fopen (filename, "r");
  if (fp == NULL)
  {
    return;
  }

  /* Read next line */
  while ((s = fgets (buff, sizeof buff, fp)) != NULL)
  {
    /* Skip blank lines and comments */
    if (buff[0] == '\n' || buff[0] == '#')
      continue;

    /* Parse name/value pair from line */
    char name[MAXLEN], value[MAXLEN];
    s = strtok (buff, "=");
    if (s==NULL)
      continue;
    else
      strncpy (name, s, MAXLEN);
    s = strtok (NULL, "=");
    if (s==NULL)
      continue;
    else
      strncpy (value, s, MAXLEN);
    trim (value);

    /* Copy into correct entry in parameters struct */
    if (strcmp(name, "win")==0)
      strncpy (win, value, MAXLEN);
    else if (strcmp(name, "lin")==0)
      strncpy (lin, value, MAXLEN);
    else if (strcmp(name, "mac")==0)
      strncpy (mac, value, MAXLEN);
    else{
        /*sprintf(gBob_debstr2,"WARNING: %s/%s: Unknown name/value pair!\n",
        name, value);
       XPLMDebugString(gBob_debstr2);*/
        AirframeDef def=AirframeDef();
        def.setData(value);
        airframeDefs.push_back(def);
    }
  }

  /* Close file */
  fclose (fp);
}
void JVM::start(void)
{
    if(!hasjvm)
        return;
    XPLMGetSystemPath(xp_path);
    sprintf(gBob_debstr2,"AutoATC Start!\n");
    
    XPLMDebugString(gBob_debstr2);
     
     
    
     jmethodID startMethod=env->GetStaticMethodID(commandsClass, "start", "()V");  // find method
     env->CallStaticVoidMethod(commandsClass, startMethod);                      // call method
     
}
void JVM::updateAirframes(void){
    if(!file_exists(CONFIG_FILE_DEFAULT_AIRFRAMES)){
         printf("ERROR:AUTOATC Can't find airframes definitions");
        XPLMDebugString("ERROR:AUTOATC Can't find airframes definitions");
     }
    else{
        airframeDefs.clear();
        parse_config(CONFIG_FILE_DEFAULT_AIRFRAMES);
        for (unsigned i=0; i<airframeDefs.size(); i++){
            sprintf(gBob_debstr2,"AutoATC Airframe %d is %s. at %f \n",i, airframeDefs[i].getPath(),airframeDefs[i].getOffset());
           // printf(gBob_debstr2);
            XPLMDebugString(gBob_debstr2);
        }
        if(airframeDefs.size()==0){
            //we dont have any definitions
            printf("no airframes found, using Resources/default scenery/airport scenery/Aircraft/Heavy_Metal/747United.obj as default\n");
            XPLMDebugString("no airframes found, using Resources/default scenery/airport scenery/Aircraft/Heavy_Metal/747United.obj as default");
            AirframeDef def=AirframeDef();
            def.setData("Resources/default scenery/airport scenery/Aircraft/Heavy_Metal/747United.obj,0.0,0,0");
            airframeDefs.push_back(def);
        }
    }
}
void JVM::stop(void)
{
    if(enabledATCPro){
            stopPlanes();
            enabledATCPro=false;
        }
    if(!hasjvm)
        return;
    jmethodID startMethod=env->GetStaticMethodID(commandsClass, "stop", "()V");  // find method
     env->CallStaticVoidMethod(commandsClass, startMethod);                      // call method
     //sprintf(gBob_debstr2,"AutoATC Stop!\n");
     //XPLMDebugString(gBob_debstr2);
}
void JVM::broadcast(void){
    if(!hasjvm)
        return;
    env->CallStaticVoidMethod(commandsClass, broadcastMethod);                      // call method
}
jstring JVM::getData(const char* reference){
    //jmethodID 
    if(!hasjvm)
        return ( jstring)"No JVM|";
    jstring stringArg1 = env->NewStringUTF(reference);
    jstring jstr = (jstring) env->CallStaticObjectMethod(commandsClass, midToString,stringArg1);
    env->DeleteLocalRef(stringArg1);
    return jstr;
}
int JVM::getStndbyFreq(int roll){
    if(!hasjvm)
        return -1;
    jint fOnehundred = (jint) env->CallStaticIntMethod(commandsClass, getStndbyMethod,roll);
    return fOnehundred;
}
void JVM::setData(jfloat planeData[]){
    if(!hasjvm)
        return;
    jfloatArray planeDataV=env->NewFloatArray(14);
    env->SetFloatArrayRegion(planeDataV, 0, 14, planeData);
    env->CallStaticVoidMethod(commandsClass, setDataMethod,planeDataV); 
    env->DeleteLocalRef(planeDataV);
}

PlaneData JVM::getPlaneData(int id){
    PlaneData retVal;
    if(!hasjvm){
        retVal.live=false;
        return retVal;
    }
    jdoubleArray    jplanedata   = (jdoubleArray) env->CallStaticObjectMethod( commandsClass, getPlaneDataMethod,id-1 );
    jdouble *element = env->GetDoubleArrayElements(jplanedata, 0);
    jsize len = env->GetArrayLength(jplanedata);
    /*double v=element[0];
    retVal.phi=(float)v;*/
    if(element[0]==0&&element[1]==0&&element[2]==0)
        {
            retVal.live=false;
            return retVal;
        }
    retVal.live=true;
    if(element[2]==-9999.0)
        return retVal;
    XPLMWorldToLocal(element[0],element[1],element[2],&retVal.x,&retVal.y,&retVal.z);
    retVal.the=(float)element[3];
    retVal.phi=(float)element[4];
    retVal.psi=(float)element[5];
    retVal.gearDown=(float)element[6];
    retVal.throttle=element[7];
    retVal.timeStamp=element[8];
    retVal.airframe=(int)element[9];
    env->ReleaseDoubleArrayElements(jplanedata,element,0);
    /*double v=planedata[0];*/
    /*jsize index(0);
    jdouble v = (jdouble)env->GetDoubleArrayElements(planedata,index);;*/
    //retVal.the=v;
    return retVal;
}
void JVM::getCommandData(){
    if(!hasjvm)
        return;
    jstring jstr = (jstring) env->CallStaticObjectMethod(commandsClass, commandString);
    char buffer[1024];
    const char* nativeString = env->GetStringUTFChars(jstr, JNI_FALSE);
    //char* astring=(char *)nativeString; 
    //sprintf(Buffer, "%s",astring); 
    size_t length = strlen(nativeString);
    if(length>0){
       XPLMCommandRef cmd= XPLMFindCommand(nativeString);
       if(cmd)
        XPLMCommandOnce(cmd);
    }
    env->ReleaseStringUTFChars(jstr, nativeString);
}
char* JVM::getModel(int id){
    if(id>=airframeDefs.size()){
        updateAirframes();

    }
    if(id>=airframeDefs.size()||id<0)
        return standbyAirframe.getPath();
    return airframeDefs[id].getPath();
}
double JVM::getOffset(int id){
    if(id>=airframeDefs.size()){
        updateAirframes();

    }
    if(id>=airframeDefs.size()||id<0)
        return standbyAirframe.getOffset();
    return airframeDefs[id].getOffset();
}
int JVM::getSound(int id){
    if(id>=airframeDefs.size()){
        updateAirframes();

    }
    if(id>=airframeDefs.size()||id<0)
        return standbyAirframe.getSound();
    return airframeDefs[id].getSound();
}
int JVM::getDrefStyle(int id){
    if(id>=airframeDefs.size()){
        updateAirframes();

    }
    if(id>=airframeDefs.size()||id<0)
        return standbyAirframe.getDrefStyle();
    return airframeDefs[id].getDrefStyle();
}
static JVM jvmO;

void initJVM(){
    
    jvmO=JVM();

}

JVM* getJVM(){
    //if(jvmO==NULL)
     //  XPLMDebugString("Fatal Error:JVM didn't persist");
    return &jvmO;
}

void JVM::createMenu(){
    if(g_menu_container_idx==-1)
        g_menu_container_idx = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "AutoATC", 0, 0);
	g_menu_id = XPLMCreateMenu("AutoATC", XPLMFindPluginsMenu(), g_menu_container_idx, menu_handler, NULL);
    //if(jvmO->hasjvm)
    //    XPLMAppendMenuItem(g_menu_id, "Toggle AI Planes", (void *)"Menu Item 3", 1);
	if(hasjvm){
        XPLMAppendMenuItem(g_menu_id, "Toggle Log", (void *)"Menu Item 1", 1);
    
        XPLMAppendMenuItem(g_menu_id, "Restart Radio", (void *)"Menu Item 2", 2);
    }
    XPLMAppendMenuItem(g_menu_id, "Settings", (void *)"Menu Item 5", 3);
    XPLMAppendMenuItem(g_menu_id, "About", (void *)"Menu Item 4", 4);
    
    if(hasjvm){
               setICAO();
               XPLMRegisterFlightLoopCallback(	SendATCData,	/* Callback */10,					/* Interval */NULL);					/* refcon not used. */
               //initPlanes();
           }
}
void JVM::destroyMenu(){
    XPLMDestroyMenu(g_menu_id);
    if(hasjvm)
        XPLMUnregisterFlightLoopCallback(SendATCData, NULL);
}
void JVM::setICAO(){
    JVM* jvmO=getJVM();
    char icao[256];
    XPLMDataRef	dr_plane_ICAO = XPLMFindDataRef ("sim/aircraft/view/acf_ICAO");
    XPLMGetDatab(dr_plane_ICAO,icao,0,40);
                
    jstring jstr = getData(icao);

    const char* nativeString = env->GetStringUTFChars(jstr, JNI_FALSE);
    char* astring=(char *)nativeString;   
    env->ReleaseStringUTFChars(jstr, nativeString);
}
void JVM::toggleLogWindow(){
    XPLMDataRef vr_dref =XPLMFindDataRef("sim/graphics/VR/enabled");
    const bool vr_is_enabled = XPLMGetDatai(vr_dref);
    if(log_window==NULL||vr_is_enabled!=loginVR){
         XPLMCreateWindow_t params;
        params.structSize = sizeof(params);
        params.visible = 1;
        params.drawWindowFunc = draw_atc_text;
        // Note on "dummy" handlers:
        // Even if we don't want to handle these events, we have to register a "do-nothing" callback for them
        params.handleMouseClickFunc = dummy_mouse_handler;
        params.handleRightClickFunc = dummy_mouse_handler;
        params.handleMouseWheelFunc = dummy_wheel_handler;
        params.handleKeyFunc = dummy_key_handler;
        params.handleCursorFunc = dummy_cursor_status_handler;
        params.refcon = NULL;
        params.layer = xplm_WindowLayerFloatingWindows;

        params.decorateAsFloatingWindow = xplm_WindowDecorationRoundRectangle;
        
        // Set the window's initial bounds
        // Note that we're not guaranteed that the main monitor's lower left is at (0, 0)...
        // We'll need to query for the global desktop bounds!
        int left, bottom, right, top;
        XPLMGetScreenBoundsGlobal(&left, &top, &right, &bottom);
        params.left = left + 50;
        params.bottom = bottom + 150;
        params.right = params.left + 400;
        params.top = params.bottom + 400;
        
        log_window = XPLMCreateWindowEx(&params);
        
        // Position the window as a "free" floating window, which the user can drag around
        loginVR=vr_is_enabled;
        if(vr_is_enabled)
        {
            XPLMSetWindowPositioningMode(log_window, xplm_WindowVR, 0);
        }
        else
            XPLMSetWindowPositioningMode(log_window, xplm_WindowPositionFree, -1);
        // Limit resizing our window: maintain a minimum width/height of 100 boxels and a max width/height of 300 boxels
        XPLMSetWindowResizingLimits(log_window, 200, 200, 500, 500);
        XPLMSetWindowTitle(log_window, "AutoATC Log");
        log_visible=true;
    }
    else if(!log_visible){
        XPLMSetWindowIsVisible(log_window,1);
        log_visible=true;
    }
     else{
         XPLMSetWindowIsVisible(log_window,0);
        log_visible=false;
     }
}
void menu_handler(void * in_menu_ref, void * in_item_ref)
{
    JVM* jvmO=getJVM();
	if(!strcmp((const char *)in_item_ref, "Menu Item 1"))
	{
		 jvmO->toggleLogWindow();
   
	}
   
	if(!strcmp((const char *)in_item_ref, "Menu Item 2")){
        if(jvmO->hasjvm){
            if(enabledATCPro){

            stopPlanes();
            }
           jvmO->start();
        }
        else
            jvmO->popupNoJVM();
    }
    /*if(!strcmp((const char *)in_item_ref, "Menu Item 3")){
         if(jvmO->hasjvm){
        if(!enabledATCPro){
            char icao[256];
            XPLMGetDatab(dr_plane_ICAO,icao,0,40);
            jstring jstr = jvmO->getData(icao);
            const char* nativeString = jvmO->env->GetStringUTFChars(jstr, JNI_FALSE);
            char* astring=(char *)nativeString;   
            jvmO->env->ReleaseStringUTFChars(jstr, nativeString);
            initPlanes();
            enabledATCPro=true;
        }else{
            stopPlanes();
            enabledATCPro=false;
        }
         }
        else
            popupNoJVM();
    }*/
    if(!strcmp((const char *)in_item_ref, "Menu Item 4"))
	{
		 
    XPLMCreateWindow_t params;
	params.structSize = sizeof(params);
	params.visible = 1;
	params.drawWindowFunc = draw_about_text;
	// Note on "dummy" handlers:
	// Even if we don't want to handle these events, we have to register a "do-nothing" callback for them
	params.handleMouseClickFunc = dummy_mouse_handler;
	params.handleRightClickFunc = dummy_mouse_handler;
	params.handleMouseWheelFunc = dummy_wheel_handler;
	params.handleKeyFunc = dummy_key_handler;
	params.handleCursorFunc = dummy_cursor_status_handler;
	params.refcon = NULL;
	params.layer = xplm_WindowLayerFloatingWindows;

	params.decorateAsFloatingWindow = xplm_WindowDecorationRoundRectangle;
	
	// Set the window's initial bounds
	// Note that we're not guaranteed that the main monitor's lower left is at (0, 0)...
	// We'll need to query for the global desktop bounds!
	int left, bottom, right, top;
	XPLMGetScreenBoundsGlobal(&left, &top, &right, &bottom);
	params.left = left + 50;
	params.bottom = bottom + 150;
	params.right = params.left + 400;
	params.top = params.bottom + 400;
	
	 XPLMWindowID	g_window = XPLMCreateWindowEx(&params);
	
	// Position the window as a "free" floating window, which the user can drag around
    XPLMDataRef vr_dref =XPLMFindDataRef("sim/graphics/VR/enabled");
    const bool vr_is_enabled = XPLMGetDatai(vr_dref);
    if(vr_is_enabled)
    {
        XPLMSetWindowPositioningMode(g_window, xplm_WindowVR, 0);
    }
    else
	    XPLMSetWindowPositioningMode(g_window, xplm_WindowPositionFree, -1);
	// Limit resizing our window: maintain a minimum width/height of 100 boxels and a max width/height of 300 boxels
	    XPLMSetWindowResizingLimits(g_window, 200, 200, 500, 500);
	    XPLMSetWindowTitle(g_window, "About AutoATC");
	}
    if(!strcmp((const char *)in_item_ref, "Menu Item 5")){
        //printf("called me 1\n");
        settings.init();//open a settings window
    }
         
}
void	draw_about_text(XPLMWindowID in_window_id, void * in_refcon)
{
	// Mandatory: We *must* set the OpenGL state before drawing
	// (we can't make any assumptions about it)
	XPLMSetGraphicsState(
						 0 /* no fog */,
						 0 /* 0 texture units */,
						 0 /* no lighting */,
						 0 /* no alpha testing */,
						 1 /* do alpha blend */,
						 1 /* do depth testing */,
						 0 /* no depth writing */
						 );
	
	int l, t, r, b;
	XPLMGetWindowGeometry(in_window_id, &l, &t, &r, &b);
	
	float col_white[] = {1.0, 1.0, 1.0}; // red, green, blue
    int ww=r-l;
    JVM* jvmO=getJVM();
    if(jvmO->hasjvm){
        jstring jstr = jvmO->getData(plugin_version);
        const char* nativeString = jvmO->env->GetStringUTFChars(jstr, JNI_FALSE);
    
        char* astring=(char *)nativeString;   
	    XPLMDrawString(col_white, l + 10, t - 20, astring, &ww, xplmFont_Proportional);
        jvmO->env->ReleaseStringUTFChars(jstr, nativeString);
    
    }
    else{
        XPLMDrawString(col_white, l + 10, t - 20, "Java VM not found, see:\n (X-Plane 11/Resources/plugins/java/defaultjvm.txt)\n For installation instructions \n", &ww, xplmFont_Proportional);
    }
}
  void	draw_atc_text(XPLMWindowID in_window_id, void * in_refcon)
{
	// Mandatory: We *must* set the OpenGL state before drawing
	// (we can't make any assumptions about it)
	XPLMSetGraphicsState(
						 0 /* no fog */,
						 0 /* 0 texture units */,
						 0 /* no lighting */,
						 0 /* no alpha testing */,
						 1 /* do alpha blend */,
						 1 /* do depth testing */,
						 0 /* no depth writing */
						 );
	
	int l, t, r, b;
	XPLMGetWindowGeometry(in_window_id, &l, &t, &r, &b);
	
	float col_white[] = {1.0, 1.0, 1.0}; // red, green, blue
    int ww=r-l;
    JVM* jvmO=getJVM();
    if(jvmO->hasjvm){
       /* const char* myName = "Default";
        char icao[256];
        XPLMGetDatab(dr_plane_ICAO,icao,0,40);*/
        jstring jstr = jvmO->getData("Console");
        const char* nativeString = jvmO->env->GetStringUTFChars(jstr, JNI_FALSE);
    
        char* astring=(char *)nativeString;   
	    XPLMDrawString(col_white, l + 10, t - 20, astring, &ww, xplmFont_Proportional);
        jvmO->env->ReleaseStringUTFChars(jstr, nativeString);
    
    }
    else{
        XPLMDrawString(col_white, l + 10, t - 20, "Java VM not found, see:\n (X-Plane 11/Resources/plugins/java/defaultjvm.txt)\n For installation instructions \n", &ww, xplmFont_Proportional);
    }
}
float SendATCData(float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon)
{
   
    JVM* jvmO=getJVM();
    if(!jvmO->hasjvm)
        return 1;
   /* if(!hot){

        jvmO->start();

        hot=true;
    }*/
    /*if(!battery_on&&hot){
        jvmO->stop();
        hot=false;
    }*/
    if(jvmO->hasjvm){
        jfloat planeData[13]={};
        planeData[0]=(jfloat)(XPLMGetDatai(transponder_codeRef)*1.0);
        planeData[1]=XPLMGetDataf(latitudeRef);
        planeData[2]=XPLMGetDataf(longitudeRef);
        planeData[3]=XPLMGetDataf(altitudeRef);
        planeData[4]=(jfloat)(XPLMGetDatai(com1_freq_hzRef)*1.0);
        planeData[5]=(jfloat)(XPLMGetDatai(com1_stdby_freq_hz)*1.0);
        planeData[6]=XPLMGetDataf(iasRef);
        planeData[7]=XPLMGetDataf(gyroHeadingRef);
        planeData[8]=XPLMGetDataf(altPressureRef);
        planeData[9]=XPLMGetDataf(mslPressureRef);
        planeData[10]=XPLMGetDataf(fpmRef);
        planeData[11]=XPLMGetDataf(pitchRef);
        planeData[12]=XPLMGetDataf(rollRef);
        planeData[13]=(jfloat)(XPLMGetDatai(battery_onRef)*1.0);
        jvmO->setData(planeData);
        PlaneData thisData=jvmO->getPlaneData(-1);
        jvmO->getCommandData();
        if(!enabledATCPro){
            
            if(thisData.live){ 
                initPlanes();
                enabledATCPro=true;
            }
        }
        else if(!thisData.live&&enabledATCPro){
            stopPlanes();
            enabledATCPro=false;
        }
    }
    return 0.05;
}
void JVM::popupNoJVM(){
     XPLMCreateWindow_t params;
	params.structSize = sizeof(params);
	params.visible = 1;
	params.drawWindowFunc = draw_jvm_text;
	// Note on "dummy" handlers:
	// Even if we don't want to handle these events, we have to register a "do-nothing" callback for them
	params.handleMouseClickFunc = dummy_mouse_handler;
	params.handleRightClickFunc = dummy_mouse_handler;
	params.handleMouseWheelFunc = dummy_wheel_handler;
	params.handleKeyFunc = dummy_key_handler;
	params.handleCursorFunc = dummy_cursor_status_handler;
	params.refcon = NULL;
	params.layer = xplm_WindowLayerFloatingWindows;

	params.decorateAsFloatingWindow = xplm_WindowDecorationRoundRectangle;
	
	// Set the window's initial bounds
	// Note that we're not guaranteed that the main monitor's lower left is at (0, 0)...
	// We'll need to query for the global desktop bounds!
	int left, bottom, right, top;
	XPLMGetScreenBoundsGlobal(&left, &top, &right, &bottom);
	params.left = left + 50;
	params.bottom = bottom + 150;
	params.right = params.left + 200;
	params.top = params.bottom + 200;
	
	XPLMWindowID	g_window = XPLMCreateWindowEx(&params);
	
	// Position the window as a "free" floating window, which the user can drag around
	XPLMSetWindowPositioningMode(g_window, xplm_WindowPositionFree, -1);
	// Limit resizing our window: maintain a minimum width/height of 100 boxels and a max width/height of 300 boxels
	XPLMSetWindowResizingLimits(g_window, 200, 200, 300, 300);
	XPLMSetWindowTitle(g_window, "Java Error");
 }
 
 
  void	draw_jvm_text(XPLMWindowID in_window_id, void * in_refcon)
{
	// Mandatory: We *must* set the OpenGL state before drawing
	// (we can't make any assumptions about it)
	XPLMSetGraphicsState(
						 0 /* no fog */,
						 0 /* 0 texture units */,
						 0 /* no lighting */,
						 0 /* no alpha testing */,
						 1 /* do alpha blend */,
						 1 /* do depth testing */,
						 0 /* no depth writing */
						 );
	
	int l, t, r, b;
	XPLMGetWindowGeometry(in_window_id, &l, &t, &r, &b);
	
	float col_white[] = {1.0, 1.0, 1.0}; // red, green, blue
    int ww=r-l;
   
        XPLMDrawString(col_white, l + 10, t - 20, "Java VM not found, see:\n (X-Plane 11/Resources/plugins/java/defaultjvm.txt)\n For installation instructions \n", &ww, xplmFont_Proportional);
    
}