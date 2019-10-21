#include <jni.h>
#include <string>
#include <cstring>
 #include "XPLMMenus.h"
  #include "XPLMDisplay.h"
   #include "XPLMDataAccess.h"
   #include <sstream>
#include <fstream>
#include <mutex>
#include <vector>
#define MAXLEN 255
class AirframeDef{
    private:
    double yOffset;
    char path[512];
    int soundIndex;
    int drefStyle;
    public:
    AirframeDef();
    void setData(std::string inLine);
    char* getPath(void);
    double getOffset(void);
    int getSound(void);
    int getDrefStyle(void);
};
class PlaneData{
public:
    bool live;
    double x;
    double y;
    double z;
    float gearDown;
    float throttle;
    float		the;
	float		phi;
	float		psi;
    double timeStamp;
    //double remoteTimestamp;
    int airframe;
};
typedef int (*JNI_CreateJavaVM_t)(void *, void *, void *);
typedef jint (*JNI_GetCreatedJavaVMs_t)(JavaVM**, jsize, jsize*);
typedef jint (*JNI_AttachCurrentThread_t)(JavaVM *vm, void **p_env, void *thr_args);
float	SendATCData(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon); 
//static  // The menu container we'll append all our menu items to                                   
void menu_handler(void * in_menu_ref, void * in_item_ref);
//static 

static XPLMDataRef  com1_stdby_freq_hz = NULL;
static XPLMDataRef  nav1_stdby_freq_hz = NULL;
static XPLMDataRef  nav2_stdby_freq_hz = NULL;
static XPLMDataRef  adf1_stdby_freq_hz = NULL;
static XPLMDataRef  sysTimeRef = NULL;
static XPLMDataRef  adf2_stdby_freq_hz = NULL;
static XPLMDataRef  com1_freq_hzRef = NULL;
static XPLMDataRef  nav1_freq_hzRef = NULL;
static XPLMDataRef  nav2_freq_hzRef = NULL;
static XPLMDataRef  adf1_freq_hzRef = NULL;
static XPLMDataRef  adf2_freq_hzRef = NULL;
static XPLMDataRef  audio_selection_nav1 = NULL;
static XPLMDataRef  audio_selection_nav2 = NULL;
static XPLMDataRef  audio_selection_adf1 = NULL;
static XPLMDataRef  audio_selection_adf2 = NULL;
static XPLMDataRef  HSI_source = NULL;
static XPLMDataRef  nav1_nav_id = NULL;
static XPLMDataRef  nav2_nav_id = NULL;
static XPLMDataRef  adf1_nav_id = NULL;
static XPLMDataRef  adf2_nav_id = NULL;
class String 
{ 
private: 
     
    int size; 
public: 
    char *s;
    String(const char *); // constructor 
    ~String();      // destructor 
}; 
  


class JVM
{
private:
  
  
  JNI_CreateJavaVM_t JNI_CreateJavaVM;
  JNI_GetCreatedJavaVMs_t JNI_GetCreatedJavaVMs;
  JNI_AttachCurrentThread_t JNI_AttachCurrentThread;
  
    // The index of our menu item in the Plugins menu
   bool log_visible;
   bool loginVR;
   jfloat planeData[14];
    XPLMWindowID	log_window = NULL;
  #if defined(__linux__)
  void *libnativehelper;
   #elif defined(_WIN64)
   HINSTANCE libnativehelper;
   #elif defined(__APPLE__)
   void *libnativehelper;
   #endif
   
   std::vector<String *> commandsList;
   char logpageString[128]={0};
   char logpageData[2048]={0};
   double getLogTime=0;
public:
    
    XPLMMenuID g_menu_id;
    int g_menu_container_idx=-1;
    char win[MAXLEN];
    char lin[MAXLEN];
    char mac[MAXLEN];
    char device[MAXLEN];
    char slave[MAXLEN];
    bool hasjvm;
    bool setIcaov;
    bool loadedLibrary=false;
    int logPage;
    float lastNavAudio;
    int lastFoundNav;
    JavaVM *jvm;                      // Pointer to the JVM (Java Virtual Machine)
    JNIEnv *env;                      // Pointer to native interface
    JNIEnv *plane_env;                      // Pointer to native interface
    jclass commandsClass;
    jclass threadcommandsClass;
    jmethodID getPlaneDataMethod;
    jmethodID getPlaneDataThreadMethod;
    jmethodID broadcastMethod;
    jmethodID setThreadDataMethod;
    jmethodID midToString;
    jmethodID midToThreadString;
    jmethodID commandString;
    jmethodID getStndbyMethod;
    AirframeDef standbyAirframe;
    bool live;
    char notepad[1024];
    JVM();
    ~JVM();
    void init_parameters (void);
    void init_parameters (char * jvmfilename);
    void parse_config (char * filename);
    void activateJVM(void);
    void deactivateJVM(void);
    void addSystemClassLoaderPath(const char* filePath);
    bool connectJVM();
    char *trim (char * s);
//class methods
    void start(void);
    void stop(void);
    void systemstop(void);
    void broadcast(void);
    int getStndbyFreq(int roll);
    jstring getData(const char*);
    char *getLogData(const char*);//inline function to get cached log window data
    void retriveLogData();//queue up log window data
    void setData(jfloat data[]);
    void setThreadData();
    PlaneData getPlaneData(int id,JNIEnv *caller_env);
    char* getModel(int id);
    double getOffset(int id);
    int getSound(int id);
    int getDrefStyle(int id);
    void createMenu();
    void destroyMenu();
    void popupNoJVM();
    void setICAO();
    void updateAirframes();
    void toggleLogWindow();
    void LogPageWindowPlus();
    void getCommandData();
    void getThreadCommandData();
    char* getDevice();
    long getisSlave();
    void setDevice(char*);
    void setisSlave(long);
    void getMorse();
    void joinThread();
    float getSysTime();
    //void testExistingJVM();
};

//static JVM jvmO;
void initJVM();
JVM *getJVM();