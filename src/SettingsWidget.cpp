
#include "XPLMPlugin.h"
 #include "XPWidgets.h"
#include "Settings.h"
 #include "XPStandardWidgets.h"
 #include "XPLMProcessing.h"
#include "jvm.h"


using namespace std;
JVM* getJVM();//jvm.cpp
static  XPWidgetID	w_window;
static XPWidgetID jvmField;
static XPWidgetID ipField;
static XPWidgetID setButton;
static XPWidgetID testButton;
static XPWidgetID resetButton;
static XPWidgetID statusCaption[5];
static XPWidgetID w_top;
static XPWidgetID w_bottom;
static bool hasSettingsWindow;
SettingsWidget::SettingsWidget(){

}
static float	displaySettingsStatus(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon); 
void SettingsWidget::setIP(char * jvmBuffer,char *ipbuffer){
    char text[512];
    sprintf(text,"Apply %s\n",jvmBuffer);
     printf(text);
     XPLMUnregisterFlightLoopCallback(displaySettingsStatus, NULL);
	XPDestroyWidget(w_window, 1);
    hasSettingsWindow=false;
}


void SettingsWidget::test(char * jvmBuffer,char *ipbuffer){
    /*char text[512];
    sprintf(text,"Test %s\n",buffer);
     printf(text);*/
     JVM* jvmO;
     try{
         
         jvmO=getJVM();
         bool lockedIn=!jvmO->loadedLibrary;
         //if(jvmO->hasjvm){
        jvmO->deactivateJVM();
         //}  
         //else{
         //initJVM();
         //jvmO=getJVM();
        jvmO->init_parameters(jvmBuffer);//"/home/mSparks/Downloads/jre1.8.0_181/lib/amd64/server/libjvm.so");
        jvmO->activateJVM();

         //}
        // jvmO->hasjvm=false;
         char com[512];
         if(jvmO->hasjvm&&lockedIn){
             #if defined(__linux__)
                sprintf(com,"Network path:lin=%s",jvmBuffer);
            #elif defined(_WIN64)
                sprintf(com,"Network path:win=%s",jvmBuffer);
            #elif defined(__APPLE__)
                sprintf(com,"Network path:mac=%s",jvmBuffer);
            #endif
             
             jstring jstr = jvmO->getData(com);
         }
          jvmO->start();
            sprintf(com,"Network mobile:phone=%s",ipbuffer);
             jstring jstr = jvmO->getData(com);
     }catch(...){
		printf("Exception during JVM test\n");
		return;
	}   
}


void SettingsWidget::reset(){
   // /home/mSparks/Downloads/jre1.8.0_181/lib/amd64/server/libjvm.so
}
void SettingsWidget::updateData(){
    JVM* jvmO;
    char Buffer[512];
    //std::string Buffer;
    try{
		jvmO=getJVM();
        if(!jvmO->hasjvm){
            XPSetWidgetDescriptor(statusCaption[0], "JVM Offline");
            XPSetWidgetDescriptor(statusCaption[1], "Update JVM location");
            XPSetWidgetDescriptor(statusCaption[2], "And press test");
            for(int i=2;i<5;i++)
                XPSetWidgetDescriptor(statusCaption[i], "  ");
            return;
        }
        jvmO->setICAO();
         jstring jstr = jvmO->getData("Network");
        const char* nativeString = jvmO->env->GetStringUTFChars(jstr, JNI_FALSE);
    
        char* astring=(char *)nativeString; 
        sprintf(Buffer, "%s",astring); 
        std::string inLine = Buffer;
        std::size_t lastFound=0;
        
        
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
    if(hasSettingsWindow)
    return;
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
      XPSetWidgetProperty(w_top, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);                            
   // XPSetWidgetProperty(w_top, xpProperty_SubWindowType, xpMainWindowStyle_Translucent);
    w_bottom = XPCreateWidget(x+50, y-102, x2-10, y2+10,
					1,	// Visible
					"Controls",	// desc
					0,		// root
					w_window,
					xpWidgetClass_SubWindow);  
      XPSetWidgetProperty(w_bottom, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);                            
  //  XPSetWidgetProperty(w_bottom, xpProperty_SubWindowType, xpMainWindowStyle_Translucent);
       XPAddWidgetCallback(w_window, SettingsWidgetsHandler); 

    XPWidgetID jvmCaption = XPCreateWidget(x+10, y-20, x+60, y-40,
					1, "JVM:", 0, w_window,
					xpWidgetClass_Caption);
//XPSetWidgetProperty(jvmCaption, xpProperty_CaptionLit, 1);
 JVM* jvmO=getJVM();
 #if defined(__linux__)
  char *lib=jvmO->lin;
   #elif defined(_WIN64)
   char *lib=jvmO->win;
   #elif defined(__APPLE__)
   char *lib=jvmO->mac;
   #endif
    jvmField = XPCreateWidget(x+70, y-20, x2-20, y-40,
					1, lib, 0, w_window,
					xpWidgetClass_TextField);
    XPSetWidgetProperty(jvmField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(jvmField, xpProperty_Enabled, !jvmO->loadedLibrary);

    XPWidgetID ipCaption = XPCreateWidget(x+10, y-50, x+60, y-60,
					1, "Device IP:", 0, w_window,
					xpWidgetClass_Caption);
    //XPSetWidgetProperty(ipCaption, xpProperty_CaptionLit, 1);
    ipField = XPCreateWidget(x+70, y-50, x2-20, y-60,
					1, "any", 0, w_window,
					xpWidgetClass_TextField);
    XPSetWidgetProperty(ipField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(ipField, xpProperty_Enabled, 1);
    
   
    

    testButton = XPCreateWidget(x2-160, y-70, x2-100, y-90,
					1, "Connect", 0, w_window,
					xpWidgetClass_Button);

    XPSetWidgetProperty(resetButton, xpProperty_ButtonType, xpPushButton);

    /*testButton = XPCreateWidget(x2-110, y-70, x2-70, y-90,
					1, "Test", 0, w_window,
					xpWidgetClass_Button);

    XPSetWidgetProperty(testButton, xpProperty_ButtonType, xpPushButton);*/

    setButton = XPCreateWidget(x2-60, y-70, x2-20, y-90,
					1, "Apply", 0, w_window,
					xpWidgetClass_Button);

    XPSetWidgetProperty(setButton, xpProperty_ButtonType, xpPushButton);
    for(int i=0;i<5;i++)
    statusCaption[i]=XPCreateWidget(x+60, y-112-i*15, x2-20, y-122-i*15,
					1, " ", 0, w_window,
					xpWidgetClass_Caption);

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
        char ipbuffer[255],jvmBuffer[255];
        XPGetWidgetDescriptor(jvmField, jvmBuffer, sizeof(jvmBuffer));
        XPGetWidgetDescriptor(ipField, ipbuffer, sizeof(ipbuffer));
        /*if (inParam1 == (intptr_t)resetButton)
        {
            printf("Reset\n");
            return 1;
        }
        else */
        if (inParam1 == (intptr_t)testButton)
        {
            settings.test(jvmBuffer,ipbuffer);
            return 1;
        }else if (inParam1 == (intptr_t)setButton)
        {
            settings.setIP(jvmBuffer,ipbuffer);
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