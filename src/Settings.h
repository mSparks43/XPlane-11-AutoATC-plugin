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
    void setIP(char * ipbuffer);
    void setSlave(int isSlave);
    void setAudioDevice(int device);
    void testAudioDevice(int device);
    //void test(char * jvmBuffer,char *ipbuffer);
    void reset();
    void init();
    void kill();
};

static SettingsWidget settings;