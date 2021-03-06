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
#include "XPLMPlugin.h"

#include "XPLMGraphics.h"

#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"
#include "XPLMMenus.h"
#include "XPLMUtilities.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"
#include "XPLMScenery.h"
#include "XPLMPlanes.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <jni.h>
#include <time.h>
#include "jvm.h"
#include "Simulation.h"
#include "aiplane.h"
#include "Settings.h"
#include "tcas.h"
#include "map_layer.h"
char gBob_debstr[128];

XPLMCommandRef BroadcastObjectCommand = NULL;
XPLMCommandRef intercomObjectCommand = NULL;
XPLMCommandRef nextComCommand = NULL;
XPLMCommandRef swapComCommand = NULL;
XPLMCommandRef prevComCommand = NULL;
XPLMCommandRef logViewCommand = NULL;
XPLMCommandRef logPageCommand = NULL;
XPLMCommandRef acarsCommand = NULL;
XPLMCommandRef playpauseCommand = NULL;
XPLMCommandRef playnextCommand = NULL;
XPLMCommandRef playbackCommand = NULL;
int TriggerBroadcastHandler(XPLMCommandRef inCommand,
                            XPLMCommandPhase inPhase,
                            void *inRefcon);
int TriggerIntercomHandler(XPLMCommandRef inCommand,
                            XPLMCommandPhase inPhase,
                            void *inRefcon);
int nextComHandler(XPLMCommandRef inCommand,
                   XPLMCommandPhase inPhase,
                   void *inRefcon);
int swapComHandler(XPLMCommandRef inCommand,
                   XPLMCommandPhase inPhase,
                   void *inRefcon);
int prevComHandler(XPLMCommandRef inCommand,
                   XPLMCommandPhase inPhase,
                   void *inRefcon);
int logViewHandler(XPLMCommandRef inCommand,
                   XPLMCommandPhase inPhase,
                   void *inRefcon);
int logPageHandler(XPLMCommandRef inCommand,
                   XPLMCommandPhase inPhase,
                   void *inRefcon);
int acarsHandler(XPLMCommandRef inCommand,
                   XPLMCommandPhase inPhase,
                   void *inRefcon);                   
int playpauseCommandHandler(XPLMCommandRef inCommand,
                            XPLMCommandPhase inPhase,
                            void *inRefcon);
int playnextCommandCommandHandler(XPLMCommandRef inCommand,
                                  XPLMCommandPhase inPhase,
                                  void *inRefcon);
int playbackCommandCommandHandler(XPLMCommandRef inCommand,
                                  XPLMCommandPhase inPhase,
                                  void *inRefcon);
// Callbacks we will register when we create our window

// Prototype for Draw Object tests
/* int     SendATCData(XPLMDrawingPhase     inPhase,    
                        int                  inIsBefore,    
                        void *               inRefcon);*/

char CONFIG_FILE_DEFAULT[] = "Resources/plugins/AutoATC/jvmsettings.txt";
//char CONFIG_FILE_USER[] = "Resources/plugins/AutoATC/jvmsettings.txt";
char CONFIG_FILE_ANDROID[] = "Resources/plugins/AutoATC/usermobilesettings.txt";

bool file_exists(const std::string &name)
{
    if (FILE *file = fopen(name.c_str(), "r"))
    {
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}

void registerDatarefs();
//void unregisterDatarefs();

PLUGIN_API int XPluginStart(
    char *outName,
    char *outSig,
    char *outDesc)
{
    // Plugin Info

    strcpy(outName, "AutoATC");
    strcpy(outSig, "zem.AutoATC.RadioTransciever");
    strcpy(outDesc, "Receive AutoATC communications and relay aircraft data");
    XPLMEnableFeature("XPLM_USE_NATIVE_WIDGET_WINDOWS", 1);
    //enabledATCPro=false;
    initJVM();
    JVM *jvmO = getJVM();
    jvmO->init_parameters();
    /*if (file_exists(CONFIG_FILE_USER))
        jvmO->parse_config(CONFIG_FILE_USER);
    else*/
    {
        jvmO->parse_config(CONFIG_FILE_DEFAULT);
    }
    if (file_exists(CONFIG_FILE_ANDROID))
        jvmO->parse_config(CONFIG_FILE_ANDROID);

    jvmO->activateJVM();

    BroadcastObjectCommand = XPLMCreateCommand("AutoATC/Transmit", "Transmit");

    XPLMRegisterCommandHandler(BroadcastObjectCommand,  // in Command name
                               TriggerBroadcastHandler, // in Handler
                               1,                       // Receive input before plugin windows.
                               (void *)0);              // inRefcon.
    intercomObjectCommand = XPLMCreateCommand("AutoATC/Intercom", "Intercom");
    XPLMRegisterCommandHandler(intercomObjectCommand,  // in Command name
                               TriggerIntercomHandler, // in Handler
                               1,                       // Receive input before plugin windows.
                               (void *)0);              // inRefcon.
    nextComCommand = XPLMCreateCommand("AutoATC/NextCom", "Next Stndby Frequency");

    XPLMRegisterCommandHandler(nextComCommand, // in Command name
                               nextComHandler, // in Handler
                               1,              // Receive input before plugin windows.
                               (void *)0);     // inRefcon.
    swapComCommand = XPLMCreateCommand("AutoATC/swapFreq", "Swap current Stndby Frequency");
    XPLMRegisterCommandHandler(swapComCommand, // in Command name
                               swapComHandler, // in Handler
                               1,              // Receive input before plugin windows.
                               (void *)0);     // inRefcon.
    prevComCommand = XPLMCreateCommand("AutoATC/PrevCom", "Previous Stndby Frequency");

    XPLMRegisterCommandHandler(prevComCommand, // in Command name
                               prevComHandler, // in Handler
                               1,              // Receive input before plugin windows.
                               (void *)0);     // inRefcon.

    logViewCommand = XPLMCreateCommand("AutoATC/toggleLog", "Toggle Log Window");

    XPLMRegisterCommandHandler(logViewCommand, // in Command name
                               logViewHandler, // in Handler
                               1,              // Receive input before plugin windows.
                               (void *)0);     // inRefcon. */
    logPageCommand = XPLMCreateCommand("AutoATC/nextLog", "Next Log Page");

    XPLMRegisterCommandHandler(logPageCommand, // in Command name
                               logPageHandler, // in Handler
                               1,              // Receive input before plugin windows.
                               (void *)0);     // inRefcon. */

   /* acarsCommand = XPLMCreateCommand("AutoATC/ACARS", "send ACARS message");

    XPLMRegisterCommandHandler(acarsCommand, // in Command name
                               acarsHandler, // in Handler
                               1,              // Receive input before plugin windows.
                               (void *)0);     // inRefcon. */

    playpauseCommand = XPLMCreateCommand("AutoATC/playpause", "Play Music");

    XPLMRegisterCommandHandler(playpauseCommand,        // in Command name
                               playpauseCommandHandler, // in Handler
                               1,                       // Receive input before plugin windows.
                               (void *)0);              // inRefcon. */
    playnextCommand = XPLMCreateCommand("AutoATC/playnext", "Next Track");

    XPLMRegisterCommandHandler(playnextCommand,               // in Command name
                               playnextCommandCommandHandler, // in Handler
                               1,                             // Receive input before plugin windows.
                               (void *)0);
    playbackCommand = XPLMCreateCommand("AutoATC/playback", "Previous Track");

    XPLMRegisterCommandHandler(playbackCommand,               // in Command name
                               playbackCommandCommandHandler, // in Handler
                               1,                             // Receive input before plugin windows.
                               (void *)0);
    com1_freq_hzRef = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_frequency_hz_833");
    nav1_freq_hzRef = XPLMFindDataRef("sim/cockpit/radios/nav1_freq_hz");
    nav2_freq_hzRef = XPLMFindDataRef("sim/cockpit/radios/nav2_freq_hz");
    adf1_freq_hzRef = XPLMFindDataRef("sim/cockpit/radios/adf1_freq_hz");
    adf2_freq_hzRef = XPLMFindDataRef("sim/cockpit/radios/adf2_freq_hz");
    com1_stdby_freq_hz = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_standby_frequency_hz_833");
    nav1_stdby_freq_hz = XPLMFindDataRef("sim/cockpit/radios/nav1_stdby_freq_hz");
    nav2_stdby_freq_hz = XPLMFindDataRef("sim/cockpit/radios/nav2_stdby_freq_hz");
    adf1_stdby_freq_hz = XPLMFindDataRef("sim/cockpit/radios/adf1_stdby_freq_hz");
    adf2_stdby_freq_hz = XPLMFindDataRef("sim/cockpit/radios/adf2_stdby_freq_hz");
    sysTimeRef = XPLMFindDataRef("sim/time/total_running_time_sec");
    
    HSI_source = XPLMFindDataRef("sim/cockpit2/radios/actuators/HSI_source_select_pilot");
    
    registerDatarefs();
    jvmO->start();
    XPLMSetActiveAircraftCount(0);


    return 1;
}
static int				g_is_acf_inited = 0;
PLUGIN_API void XPluginStop(void)
{
    XPLMDebugString("AUTOATC: XPluginStop\n"); 
    JVM *jvmO = getJVM();
    jvmO->unregisterFlightLoop();//make sure the flight loop is stopped
    
    if (jvmO->hasjvm)
    {
        XPLMDebugString("AUTOATC: jvm systemstop\n");
        jvmO->systemstop();
        
    }
    XPLMDebugString("AUTOATC: deactivateJVM\n");
    jvmO->deactivateJVM();//this is !hasJVM safe and needed to kill the threads
    g_is_acf_inited = 0;
    MapLayerStop();
    //unregisterDatarefs();
}

PLUGIN_API void XPluginDisable(void)
{
    XPLMDebugString("AUTOATC: XPluginDisable\n"); 
    JVM *jvmO = getJVM();
    
    jvmO->unregisterFlightLoop();
    if (jvmO->hasjvm)
    {
        
        XPLMDebugString("AUTOATC: systemstop\n");
        jvmO->systemstop();
       // jvmO->deactivateJVM();
    }
    TCASAPI* tcasAPI=getTCASAPI();
    tcasAPI->Disable();
}

#define MSG_ADD_DATAREF 0x01000000           //  Add dataref to DRE message
PLUGIN_API int XPluginEnable(void)
{
    JVM *jvmO = getJVM();
    // enabledATCPro=false;
    //return 1;
    if (!jvmO->hasjvm)
    { //try and find it

        //initJVM();
        // JVM* jvmO=getJVM();
        jvmO->init_parameters();
        /*if (file_exists(CONFIG_FILE_USER))
            jvmO->parse_config(CONFIG_FILE_USER);
        else*/
        {
            jvmO->parse_config(CONFIG_FILE_DEFAULT);
        }
    }
    jvmO->activateJVM();
    jvmO->start();
    jvmO->registerFlightLoop();
    TCASAPI* tcasAPI=getTCASAPI();
    tcasAPI->Enable();
    MapLayerEnable();
    return 1;
}

PLUGIN_API void XPluginReceiveMessage(
    XPLMPluginID inFromWho,
    long inMessage,
    void *inParam)
{
    if (inMessage == XPLM_MSG_PLANE_LOADED){
        if(inParam == 0)
			g_is_acf_inited = 0;
        XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"autoatc/aircraft/id");  //tell dref editor about it
        XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"autoatc/aircraft/af");  //tell dref editor about it
        XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"autoatc/aircraft/x");  //tell dref editor about it
        XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"autoatc/aircraft/y");  //tell dref editor about it
        XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"autoatc/aircraft/z");  //tell dref editor about it
        XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"autoatc/aircraft/target_damage");  //tell dref editor about it
        XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"autoatc/acars/in");  //tell dref editor about it
        XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"autoatc/yousay");
        XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"autoatc/acars/out");  //tell dref editor about it
        XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"autoatc/cdu");  //tell dref editor about it
        XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"autoatc/acars/received");  //tell dref editor about it
        XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"autoatc/acars/online");  //tell dref editor about it
        XPLMSendMessageToPlugin(XPLM_NO_PLUGIN_ID , MSG_ADD_DATAREF, (void*)"autoatc/logpage");
    }
    else if (inMessage == XPLM_MSG_PLANE_UNLOADED){
        g_is_acf_inited = 0;
    }else if (inMessage == XPLM_MSG_AIRPORT_LOADED){
        if(!g_is_acf_inited)
		{
            JVM *jvmO = getJVM();
            jvmO->setICAO();
            g_is_acf_inited = 1;
        }
    }
}

int TriggerBroadcastHandler(XPLMCommandRef inCommand,
                            XPLMCommandPhase inPhase,
                            void *inRefcon)
{
    JVM *jvmO = getJVM();
    if (jvmO->hasjvm)
    {
        jvmO->broadcast(false);
        //jvmO->testExistingJVM();
    }
    return 0;
}
int TriggerIntercomHandler(XPLMCommandRef inCommand,
                            XPLMCommandPhase inPhase,
                            void *inRefcon)
{
    JVM *jvmO = getJVM();
    if (jvmO->hasjvm)
    {
        jvmO->broadcast(true);
        //jvmO->testExistingJVM();
    }
    return 0;
}

int roll = 0;
void setStndby()
{
    JVM *jvmO = getJVM();
    int nf = jvmO->getStndbyFreq(roll);
    if (nf > 0)
    {
        if (jvmO->logPage == 3){
            int selected=XPLMGetDatai(HSI_source);
            if(selected==1)
                XPLMSetDatai(nav2_stdby_freq_hz, nf/10);
            else
                XPLMSetDatai(nav1_stdby_freq_hz, nf/10);
        }
        else if (jvmO->logPage == 4){
            int selected=XPLMGetDatai(HSI_source);
            if(selected==1)
                XPLMSetDatai(adf2_stdby_freq_hz, nf/10);
            else
                XPLMSetDatai(adf1_stdby_freq_hz, nf/10);
        }
        else
            XPLMSetDatai(com1_stdby_freq_hz, nf);
    }
}
int nextComHandler(XPLMCommandRef inCommand,
                   XPLMCommandPhase inPhase,
                   void *inRefcon)
{
    if (inPhase == xplm_CommandBegin)
    {
        JVM *jvmO = getJVM();
        if (jvmO->hasjvm)
        {
            roll++;
            if (roll > 350)
                roll = 0;
            setStndby();
        }
    }
    return 0;
}
int swapComHandler(XPLMCommandRef inCommand,
                   XPLMCommandPhase inPhase,
                   void *inRefcon)
{
    if (inPhase == xplm_CommandBegin)
    {
        JVM *jvmO = getJVM();
        if (jvmO->hasjvm)
        {
            if(jvmO->logPage==3){
                int selected=XPLMGetDatai(HSI_source);
                XPLMDataRef stndby=nav1_stdby_freq_hz;
                XPLMDataRef freq=nav1_freq_hzRef;
                if(selected==1){
                    stndby=nav2_stdby_freq_hz;
                    freq=nav2_freq_hzRef;
                }
                int nf=XPLMGetDatai(stndby);
                int of=XPLMGetDatai(freq);
                XPLMSetDatai(stndby, of);
                XPLMSetDatai(freq, nf);
            } 
            else if(jvmO->logPage==4){
               // int selected=XPLMGetDatai(HSI_source);
                XPLMDataRef stndby=adf1_stdby_freq_hz;
                XPLMDataRef freq=adf1_freq_hzRef;
                /*if(selected=1){
                    stndby=adf2_stdby_freq_hz;
                    freq=adf2_freq_hzRef;
                }*///make this customisable by aircraft
                int nf=XPLMGetDatai(stndby);
                int of=XPLMGetDatai(freq);
                XPLMSetDatai(stndby, of);
                XPLMSetDatai(freq, nf);
            }
            else{
                int nf=XPLMGetDatai(com1_stdby_freq_hz);
                int of=XPLMGetDatai(com1_freq_hzRef);
                XPLMSetDatai(com1_stdby_freq_hz, of);
                XPLMSetDatai(com1_freq_hzRef, nf);
            }           
        }
    }
    return 0;
}
int prevComHandler(XPLMCommandRef inCommand,
                   XPLMCommandPhase inPhase,
                   void *inRefcon)
{
    if (inPhase == xplm_CommandBegin)
    {
        JVM *jvmO = getJVM();
        if (jvmO->hasjvm)
        {
            roll--;
            if (roll < 0)
                roll = 350;
            setStndby();
        }
    }
    return 0;
}
int logViewHandler(XPLMCommandRef inCommand,
                   XPLMCommandPhase inPhase,
                   void *inRefcon)
{
    if (inPhase == xplm_CommandBegin)
    {
        JVM *jvmO = getJVM();
        jvmO->toggleLogWindow();
    }
    return 0;
}
int logPageHandler(XPLMCommandRef inCommand,
                   XPLMCommandPhase inPhase,
                   void *inRefcon)
{
    if (inPhase == xplm_CommandBegin)
    {
        JVM *jvmO = getJVM();
        jvmO->LogPageWindowPlus();
    }
    return 0;
}
int acarsHandler(XPLMCommandRef inCommand,
                   XPLMCommandPhase inPhase,
                   void *inRefcon)
{
    if (inPhase == xplm_CommandBegin)
    {
        JVM *jvmO = getJVM();
        jvmO->processAcars();
    }
    return 0;
}
int playpauseCommandHandler(XPLMCommandRef inCommand,
                            XPLMCommandPhase inPhase,
                            void *inRefcon)
{
    if (inPhase == xplm_CommandBegin)
    {
        JVM *jvmO = getJVM();
        jvmO->getData("doCommand:playpause");
    }
    return 0;
}
int playnextCommandCommandHandler(XPLMCommandRef inCommand,
                                  XPLMCommandPhase inPhase,
                                  void *inRefcon)
{
    if (inPhase == xplm_CommandBegin)
    {
        JVM *jvmO = getJVM();
        jvmO->getData("doCommand:nextTrack");
    }
    return 0;
}
int playbackCommandCommandHandler(XPLMCommandRef inCommand,
                                  XPLMCommandPhase inPhase,
                                  void *inRefcon)
{
    if (inPhase == xplm_CommandBegin)
    {
        JVM *jvmO = getJVM();
        jvmO->getData("doCommand:backTrack");
    }
    return 0;
}
