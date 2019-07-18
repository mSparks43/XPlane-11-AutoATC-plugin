 #include "XPWidgets.h"
 #include <string>
 class SettingsWidget
{
private:
    
   
    
    std::string line[5];
    

    int x=100;
    int y=500;
    int w=300;
    int h=200;
    /*static int	SettingsWidgetsHandler(
						XPWidgetMessage			inMessage,
						XPWidgetID				inWidget,
						intptr_t				inParam1,
						intptr_t				inParam2);*/
public:
    
    SettingsWidget(void);
    void updateData();
    void setIP(char * jvmBuffer,char * ipbuffer);
    void setSlave(int isSlave);
    void test(char * jvmBuffer,char *ipbuffer);
    void reset();
    void init();
    void kill();
};

static SettingsWidget settings;