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
#include "XPWidgets.h"
#include "Settings.h"
#include "XPStandardWidgets.h"
#include "XPLMProcessing.h"
#include "jvm.h"
#include <string.h>


JVM* getJVM();//jvm.cpp
static  XPWidgetID	w_window;
//static XPWidgetID jvmField;
static XPWidgetID ipField;
static XPWidgetID isSlaveField;
static XPWidgetID audioDeviceField;
static XPWidgetID setButton;
//static XPWidgetID testButton;
static XPWidgetID resetButton;
static XPWidgetID statusCaption[5];
static XPWidgetID w_top;
static XPWidgetID w_bottom;
static XPWidgetID ipCaption;
static bool hasSettingsWindow;
SettingsWidget::SettingsWidget(){

}
static float	displaySettingsStatus(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon); 
void SettingsWidget::setIP(char *ipbuffer){
   /*char text[512];
    sprintf(text,"Apply %s\n",jvmBuffer);
     printf(text);
     char gBob_debstr2[128];
     sprintf(gBob_debstr2,text);*/
     //XPLMDebugString(gBob_debstr2); 
     char com[512];
     sprintf(com,"Network mobile:phone=%s",ipbuffer);
      JVM* jvmO;
      jvmO=getJVM();
      if(!jvmO->hasjvm)
        return;
     jvmO->getData(com);
     /*XPLMUnregisterFlightLoopCallback(displaySettingsStatus, NULL);
	XPDestroyWidget(w_window, 1);
    hasSettingsWindow=false;*/
}
void SettingsWidget::setSlave(int isSlave){
     std::string com="Network mobile:isSlave=";
    if(isSlave)
        com=com+"true";
    else
        com=com+"false";
      JVM* jvmO;
      jvmO=getJVM();
      if(!jvmO->hasjvm)
        return;
     jvmO->getData(com.c_str());
     jvmO->setisSlave((long)isSlave);
}
void SettingsWidget::setAudioDevice(int audioDevice){
     std::string com="Network mobile:audioDevice=";
      com=com+std::to_string(audioDevice);
      printf("setting audio device:%s\n",com.c_str());
      JVM* jvmO;
      jvmO=getJVM();
      if(!jvmO->hasjvm)
        return;
     jvmO->getData(com.c_str());
}
void SettingsWidget::testAudioDevice(int audioDevice){
     std::string com="Network mobile:testAudioDevice=";
      com=com+std::to_string(audioDevice);
      printf("setting audio device:%s\n",com.c_str());
      JVM* jvmO;
      jvmO=getJVM();
      if(!jvmO->hasjvm)
        return;
     jvmO->getData(com.c_str());
}


void SettingsWidget::reset(){
   // /home/mSparks/Downloads/jre1.8.0_181/lib/amd64/server/libjvm.so
}
void SettingsWidget::updateData(){
    JVM* jvmO;
    char Buffer[1024]={0};
    //std::string Buffer;
    try{
		jvmO=getJVM();
        if(!jvmO->hasjvm){
            XPSetWidgetDescriptor(statusCaption[0], "JVM Offline");
            XPSetWidgetDescriptor(statusCaption[1], "Update JVM location");

            for(int i=2;i<5;i++)
                XPSetWidgetDescriptor(statusCaption[i], "  ");
            return;
        }
        //jvmO->setICAO();
        jstring jstr = jvmO->getStringData("Network");
        const char* nativeString = jvmO->env->GetStringUTFChars(jstr, JNI_FALSE);
    
        char* astring=(char *)nativeString; 
        sprintf(Buffer, "%s",astring); 
        std::string inLine = Buffer;
        std::size_t lastFound=0;
        //char gBob_debstr2[1024]={0};
        //sprintf(gBob_debstr2,Buffer);
        //XPLMDebugString(gBob_debstr2); 
        
        for(int i=0;i<5;i++){
           
            
            if(lastFound<inLine.length()){
                std::size_t found = inLine.find("|",lastFound);
                if(i==0)
                    line[i] = inLine.substr (0,found);
                else
                    line[i] = inLine.substr (lastFound,found-lastFound);
                XPSetWidgetDescriptor(statusCaption[i], line[i].c_str());
                lastFound=(found+1);
            }
            else{
                XPSetWidgetDescriptor(statusCaption[i], "  ");
            }
            
        }
        //printf(Buffer);
	    
        jvmO->env->ReleaseStringUTFChars(jstr, nativeString);
	}catch(...){
		printf("Exception getting plane data\n");
		return;
	}
}
static int SettingsWidgetsHandler(
						XPWidgetMessage			inMessage,
						XPWidgetID				inWidget,
						intptr_t				inParam1,
						intptr_t				inParam2);

void SettingsWidget::init(){
   // printf("called me 2\n");
   int x2 = x + w;
	int y2 = y - h;
    JVM* jvmO=getJVM();
    if(hasSettingsWindow){
        char* device=jvmO->getDevice();
        XPSetWidgetDescriptor(ipField, device);
        return;
    }
     hasSettingsWindow=true;
	w_window = XPCreateWidget(x, y, x2, y2,
					1,	// Visible
					"AutoATC Settings",	// desc
					1,		// root
					NULL,	// no container
					xpWidgetClass_MainWindow);
    XPSetWidgetProperty(w_window, xpProperty_MainWindowHasCloseBoxes, 1);
    XPSetWidgetProperty(w_window, xpProperty_MainWindowType, xpMainWindowStyle_Translucent);
	w_top = XPCreateWidget(x+10, y-20, x2-10, y-98,
					1,	// Visible
					"Device IP",	// desc
					0,		// root
					w_window,
					xpWidgetClass_SubWindow);  
      XPSetWidgetProperty(w_top, xpProperty_SubWindowType, xpSubWindowStyle_Screen);                            
   // XPSetWidgetProperty(w_top, xpProperty_SubWindowType, xpMainWindowStyle_Translucent);
    w_bottom = XPCreateWidget(x+50, y-102, x2-10, y2+10,
					1,	// Visible
					"Controls",	// desc
					0,		// root
					w_window,
					xpWidgetClass_SubWindow);  
      XPSetWidgetProperty(w_bottom, xpProperty_SubWindowType, xpSubWindowStyle_Screen);                            
  //  XPSetWidgetProperty(w_bottom, xpProperty_SubWindowType, xpMainWindowStyle_Translucent);
       XPAddWidgetCallback(w_window, SettingsWidgetsHandler); 
   XPWidgetID audioCaption=XPCreateWidget(x+10, y-25, x+60, y-40,
					1, "Audio Device:", 0, w_window,
					xpWidgetClass_Caption);
    XPSetWidgetProperty(audioCaption, xpProperty_CaptionLit, 1);
   audioDeviceField = XPCreateWidget(x+90, y-26, x2-15, y-41,
					1,	// Visible
					"audio device",	// desc
					0,		// root
					w_window,
					xpWidgetClass_ScrollBar);
    XPLMDataRef num_device = XPLMFindDataRef("autoatc/audiodevice");
    XPLMDataRef num_devices = XPLMFindDataRef("autoatc/num_audiodevices");                 
    XPSetWidgetProperty(audioDeviceField, xpProperty_ScrollBarType, xpScrollBarTypeScrollBar);
    XPSetWidgetProperty(audioDeviceField, xpProperty_ScrollBarMin, 0);
    XPSetWidgetProperty(audioDeviceField, xpProperty_ScrollBarMax,XPLMGetDatai(num_devices));
    XPSetWidgetProperty(audioDeviceField, xpProperty_ScrollBarSliderPosition,XPLMGetDatai(num_device));
    XPAddWidgetCallback(audioDeviceField, SettingsWidgetsHandler); 
    


    ipCaption=XPCreateWidget(x+10, y-50, x+60, y-60,
					1, "Android IP:", 0, w_window,
					xpWidgetClass_Caption);
    XPSetWidgetProperty(ipCaption, xpProperty_CaptionLit, 1);
    char* device=jvmO->getDevice();
    ipField = XPCreateWidget(x+80, y-45, x2-20, y-65,
					1, device, 0, w_window,
					xpWidgetClass_TextField);
    XPSetWidgetProperty(ipField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(ipField, xpProperty_Enabled, 1);
    
    isSlaveField = XPCreateWidget(x+10, y-70, x+50, y-100, 1, "", 0, w_window, xpWidgetClass_Button);
    XPSetWidgetProperty(isSlaveField, xpProperty_ButtonType, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(isSlaveField, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    long isSlave=jvmO->getisSlave();
    XPSetWidgetProperty(isSlaveField, xpProperty_ButtonState, isSlave);
     XPWidgetID ecCaption =XPCreateWidget(x+51, y-70, x+160, y-100,
					1, "Extended Cockpit", 0, w_window,
					xpWidgetClass_Caption);
    XPSetWidgetProperty(ecCaption, xpProperty_CaptionLit, 1);    
    XPSetWidgetProperty(resetButton, xpProperty_ButtonType, xpPushButton);

    setButton = XPCreateWidget(x2-80, y-70, x2-20, y-90,
					1, "Connect", 0, w_window,
					xpWidgetClass_Button);

    XPSetWidgetProperty(setButton, xpProperty_ButtonType, xpPushButton);
    for(int i=0;i<5;i++){
        statusCaption[i]=XPCreateWidget(x+60, y-112-i*15, x2-20, y-122-i*15,
					1, " ", 0, w_window,
					xpWidgetClass_Caption);
         XPSetWidgetProperty(statusCaption[i], xpProperty_CaptionLit, 1);            
    }

                    //xpWidgetClass_TextField);
   // XPSetWidgetProperty(statusCaption, xpProperty_TextFieldType, xpTextEntryField);
	//XPSetWidgetProperty(statusCaption, xpProperty_Enabled, 0);
    XPLMRegisterFlightLoopCallback(	displaySettingsStatus,/* Callback */1.0,	/* Interval */NULL);/* refcon not used. */
}


/*void SettingsWidget::kill(){
    hasSettingsWindow=false;
    XPLMUnregisterFlightLoopCallback(displaySettingsStatus, NULL);
}*/

int SettingsWidgetsHandler(
						XPWidgetMessage			inMessage,
						XPWidgetID				inWidget,
						intptr_t				inParam1,
						intptr_t				inParam2){

    if (inMessage == xpMessage_CloseButtonPushed)
	{
        XPLMUnregisterFlightLoopCallback(displaySettingsStatus, NULL);
		XPDestroyWidget(w_window, 1);
        hasSettingsWindow=false;
		return 1;
	}
    if (inMessage == xpMsg_PushButtonPressed)
	{
        char ipbuffer[255];//,jvmBuffer[255];
        //XPGetWidgetDescriptor(jvmField, jvmBuffer, sizeof(jvmBuffer));
        XPGetWidgetDescriptor(ipField, ipbuffer, sizeof(ipbuffer));
        /*if (inParam1 == (intptr_t)resetButton)
        {
            printf("Reset\n");
            return 1;
        }
        else */
       /* if (inParam1 == (intptr_t)testButton)
        {
            settings.test(jvmBuffer,ipbuffer);
            return 1;
        }else */
        if (inParam1 == (intptr_t)setButton)
        {
            settings.setIP(ipbuffer);
            int tmp = (int)XPGetWidgetProperty(audioDeviceField, xpProperty_ScrollBarSliderPosition, NULL);
            settings.testAudioDevice(tmp);
            return 1;
        }
    }
    if (inMessage == xpMsg_ButtonStateChanged){
        if (inParam1 == (intptr_t)isSlaveField)
        {
            int tmp = (int)XPGetWidgetProperty(isSlaveField, xpProperty_ButtonState, NULL);
            settings.setSlave(tmp);
            return 1;
        }
    }
    if (inMessage == xpMsg_ScrollBarSliderPositionChanged){
        printf("slider changed\n");
        if (inParam1 == (intptr_t)audioDeviceField)
        {
            int tmp = (int)XPGetWidgetProperty(audioDeviceField, xpProperty_ScrollBarSliderPosition, NULL);
            settings.setAudioDevice(tmp);
            return 1;
        }
    }
     return 0;                  
}
float	displaySettingsStatus(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon)
{
    settings.updateData();
    return 1.0;
}
