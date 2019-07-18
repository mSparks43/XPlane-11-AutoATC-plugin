#include <jni.h>
#include <string>
 #include "XPLMMenus.h"
  #include "XPLMDisplay.h"
   #include "XPLMDataAccess.h"
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
static XPLMDataRef  com1_freq_hzRef = NULL;
class JVM
{
private:
  
  JavaVM *jvm;                      // Pointer to the JVM (Java Virtual Machine)
  JNI_CreateJavaVM_t JNI_CreateJavaVM;
  JNI_GetCreatedJavaVMs_t JNI_GetCreatedJavaVMs;
  JNI_AttachCurrentThread_t JNI_AttachCurrentThread;
    // The index of our menu item in the Plugins menu
   bool log_visible;
   bool loginVR;
    XPLMWindowID	log_window = NULL;
  #if defined(__linux__)
  void *libnativehelper;
   #elif defined(_WIN64)
   HINSTANCE libnativehelper;
   #elif defined(__APPLE__)
   void *libnativehelper;
   #endif
public:
    XPLMMenuID g_menu_id;
    int g_menu_container_idx=-1;
    char win[MAXLEN];
    char lin[MAXLEN];
    char mac[MAXLEN];
    bool hasjvm;
    bool setIcaov;
    bool loadedLibrary=false;
    JNIEnv *env;                      // Pointer to native interface
    jclass commandsClass;
    jmethodID getPlaneDataMethod;
    jmethodID broadcastMethod;
    jmethodID setDataMethod;
    jmethodID midToString;
    jmethodID commandString;
    jmethodID getStndbyMethod;
    AirframeDef standbyAirframe;
    JVM();
    void init_parameters (void);
    void init_parameters (char * jvmfilename);
    void parse_config (char * filename);
    void activateJVM(void);
    void deactivateJVM(void);
    char *trim (char * s);
//class methods
    void start(void);
    void stop(void);
    void broadcast(void);
    int getStndbyFreq(int roll);
    jstring getData(const char*);
    void setData(jfloat data[]);
    PlaneData getPlaneData(int id);
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
    void getCommandData();
    void testExistingJVM();
};

//static JVM jvmO;
void initJVM();
JVM *getJVM();