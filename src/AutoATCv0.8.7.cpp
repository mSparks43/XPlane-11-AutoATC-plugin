   
 #include "XPLMPlugin.h"

 #include "XPLMGraphics.h"

 #include "XPLMProcessing.h"
 #include "XPLMDataAccess.h"
 #include "XPLMMenus.h"
 #include "XPLMUtilities.h"
 #include "XPWidgets.h"
 #include "XPStandardWidgets.h"
 #include "XPLMScenery.h"
 #include <string.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <ctype.h>
 #include <math.h>
 #include <jni.h>
 #include "xplane.h"
 #include <time.h>
 #include "vec_opps.h"
 #include "aiplane.h"
#include "jvm.h"
#include "Settings.h"



char gBob_debstr[128];

 


 XPLMCommandRef BroadcastObjectCommand = NULL;
XPLMCommandRef nextComCommand = NULL;
XPLMCommandRef prevComCommand = NULL;
XPLMCommandRef logViewCommand = NULL;
  int	TriggerBroadcastHandler(XPLMCommandRef   inCommand,    
                            XPLMCommandPhase      inPhase,    
                            void *                inRefcon);
  int	nextComHandler(XPLMCommandRef   inCommand,    
                            XPLMCommandPhase      inPhase,    
                            void *                inRefcon);
 int	prevComHandler(XPLMCommandRef   inCommand,    
                            XPLMCommandPhase      inPhase,    
                            void *                inRefcon);
  int	logViewHandler(XPLMCommandRef   inCommand,    
                            XPLMCommandPhase      inPhase,    
                            void *                inRefcon);
 
// Callbacks we will register when we create our window

 
 // Prototype for Draw Object tests
/* int     SendATCData(XPLMDrawingPhase     inPhase,    
                        int                  inIsBefore,    
                        void *               inRefcon);*/

char CONFIG_FILE_DEFAULT[] ="Resources/plugins/java/defaultjvm.txt";
char CONFIG_FILE_USER[] ="Resources/plugins/java/jvmsettings.txt";
char CONFIG_FILE_ANDROID[] ="Resources/plugins/java/usermobilesettings.txt";
//char* CONFIG_FILE_USER_AIRFRAMES ="Resources/plugins/java/airframes_user.txt";
 bool file_exists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

void registerDatarefs();

 PLUGIN_API int XPluginStart(
                             char *     outName,
                             char *     outSig,
                             char *     outDesc)
 {
 // Plugin Info 
   
    
    strcpy(outName, "AutoATC");
    strcpy(outSig, "zem.AutoATC.RadioTransciever");
    strcpy(outDesc, "Receive AutoATC communications and relay aircraft data");
    XPLMEnableFeature("XPLM_USE_NATIVE_WIDGET_WINDOWS", 1);
    //enabledATCPro=false;
    initJVM();
    JVM* jvmO=getJVM();
    jvmO->init_parameters();
     if(file_exists(CONFIG_FILE_USER))
        jvmO->parse_config(CONFIG_FILE_USER);
     else{
        jvmO->parse_config(CONFIG_FILE_DEFAULT);
    }
    if(file_exists(CONFIG_FILE_ANDROID))
        jvmO->parse_config(CONFIG_FILE_ANDROID);
 
    jvmO->activateJVM();
   // jvmO->stop();
     BroadcastObjectCommand = XPLMCreateCommand("AutoATC/Transmit", "Transmit");

    XPLMRegisterCommandHandler(BroadcastObjectCommand,          // in Command name
                            TriggerBroadcastHandler,       // in Handler
                            1,                              // Receive input before plugin windows.
                            (void *) 0);                    // inRefcon. 

    nextComCommand = XPLMCreateCommand("AutoATC/NextCom", "Next Stndby Frequency");

    XPLMRegisterCommandHandler(nextComCommand,          // in Command name
                            nextComHandler,       // in Handler
                            1,                              // Receive input before plugin windows.
                            (void *) 0);                    // inRefcon. 
     prevComCommand = XPLMCreateCommand("AutoATC/PrevCom", "Previous Stndby Frequency");

    XPLMRegisterCommandHandler(prevComCommand,          // in Command name
                            prevComHandler,       // in Handler
                            1,                              // Receive input before plugin windows.
                            (void *) 0);                    // inRefcon. 


    logViewCommand = XPLMCreateCommand("AutoATC/toggleLog", "Toggle Log Window");

    XPLMRegisterCommandHandler(logViewCommand,          // in Command name
                            logViewHandler,       // in Handler
                            1,                              // Receive input before plugin windows.
                            (void *) 0);                    // inRefcon. */
     com1_freq_hzRef = XPLMFindDataRef ("sim/cockpit/radios/com1_freq_hz");
    com1_stdby_freq_hz = XPLMFindDataRef ("sim/cockpit/radios/com1_stdby_freq_hz");
     
    registerDatarefs();
    jvmO->start();
  
     return 1;
 }
 
 
 PLUGIN_API void	XPluginStop(void)
 {
     JVM* jvmO=getJVM();
     if(jvmO->hasjvm){
       jvmO->stop();
       jvmO->deactivateJVM();

        
     }	
     	

 }
 
 
 PLUGIN_API void XPluginDisable(void)
 {
     JVM* jvmO=getJVM();
     if(jvmO->hasjvm){
        jvmO->deactivateJVM();
        
     }		
 }
 
 
 PLUGIN_API int XPluginEnable(void)
 {
     JVM* jvmO=getJVM();
    // enabledATCPro=false;
     //return 1;
     if(!jvmO->hasjvm){//try and find it

       //initJVM();
       // JVM* jvmO=getJVM();
        jvmO->init_parameters();
         if(file_exists(CONFIG_FILE_USER))
             jvmO->parse_config(CONFIG_FILE_USER);
         else{
             jvmO->parse_config(CONFIG_FILE_DEFAULT);
        }


     }
     jvmO->activateJVM();

     return 1;
 }
 
 
 PLUGIN_API void XPluginReceiveMessage(
                                       XPLMPluginID   inFromWho,
                                       long           inMessage,
                                       void *         inParam)
 {
 }
 
 
 
 
 int    TriggerBroadcastHandler(XPLMCommandRef     inCommand,    
                                   XPLMCommandPhase    inPhase,    
                                   void *              inRefcon)
 {
     JVM* jvmO=getJVM();
    if(jvmO->hasjvm)
        {
            jvmO->broadcast();
            //jvmO->testExistingJVM();
        }
     return 0;
 }
 int roll=0;
 void setStndby(){
     JVM* jvmO=getJVM();
        int nf=jvmO->getStndbyFreq(roll);
        if(nf>0)
            XPLMSetDatai(com1_stdby_freq_hz, nf);
 }
  int    nextComHandler(XPLMCommandRef     inCommand,    
                                   XPLMCommandPhase    inPhase,    
                                   void *              inRefcon)
 {
     if(inPhase==xplm_CommandBegin){
     JVM* jvmO=getJVM();
    if(jvmO->hasjvm)
        {
            roll++;
            if(roll>50)
                roll=0;
            setStndby();
        }
    }
     return 0;
 }
   int    prevComHandler(XPLMCommandRef     inCommand,    
                                   XPLMCommandPhase    inPhase,    
                                   void *              inRefcon)
 {
     if(inPhase==xplm_CommandBegin){
     JVM* jvmO=getJVM();
    if(jvmO->hasjvm)
        {
            roll--;
            if(roll<0)
                roll=50;
            setStndby();
        }
     }
     return 0;
 }
    int    logViewHandler(XPLMCommandRef     inCommand,    
                                   XPLMCommandPhase    inPhase,    
                                   void *              inRefcon)
 {
     if(inPhase==xplm_CommandBegin){
     JVM* jvmO=getJVM();
     jvmO->toggleLogWindow();
     }
     return 0;
 }


  
