#include "XPLMDataAccess.h"

#include <cstring>

static const std::size_t TARGETS = 31;
const float fttomtr = 0.3048f;

class TCASAPI
{
    private:
    public:
    XPLMDataRef id = NULL;
    XPLMDataRef flt_id = NULL;
    XPLMDataRef xRef = NULL;
    XPLMDataRef yRef = NULL;
    XPLMDataRef zRef = NULL;
    XPLMDataRef psiRef = NULL;
    XPLMDataRef icaoType = NULL;
    XPLMDataRef modeC_code = NULL;
    XPLMDataRef override = NULL;

// datarefs for our own plane
    //XPLMDataRef psi = NULL;
    //XPLMDataRef ele = NULL;
    const float nmtomtr = 1852.f;
    
    bool plugin_owns_tcas = false;
    int      ids[TARGETS]    = {0XA51B60,0XA51B61,0XA51B62,0XA51B63, 0XA51B64,0XA51B65,0XA51B66,0XA51B67,0XA51B68,0XA51B69,0XA51B6A,0XA51B6B,0XA51B6C,0XA51B6D,0XA51B6E,0XA51B6F,0XA51B70,0XA51B71,0XA51B72,0XA51B73, 0XA51B74,0XA51B75,0XA51B76,0XA51B77,0XA51B78,0XA51B79,0XA51B7A,0XA51B7B,0XA51B7C,0XA51B7D  };    // Required: unique ID for the target. Must be 24bit number. 
    
    
    char tailnum[TARGETS][8] = {"TEST1", "TEST2", "TEST3", "TEST4", "TEST5", "TEST6", "TEST7", "TEST8", "TEST9", "TEST10","TEST11", "TEST12", "TEST13", "TEST14", "TEST15", "TEST16", "TEST17", "TEST18", "TEST19", "TEST20","TEST21", "TEST22", "TEST23", "TEST24", "TEST25", "TEST26", "TEST27", "TEST28", "TEST29", "TEST30"};       // Optional: Flight ID is item 7 of the ICAO flightplan. So it can be the tailnumber OR the flightnumber! Cannot be longer than 7 chars+nullbyte!
    char icaonum[TARGETS][8] = {"B744", "C172", "B744", "C172", "B744", "C172", "B744", "C172", "B744", "C172","B744", "C172", "B744", "C172", "B744", "C172", "B744", "C172", "B744", "C172","B744", "C172", "B744", "C172", "B744", "C172", "B744", "C172", "B744", "C172"};       // Optional: Flight ID is item 7 of the ICAO flightplan. So it can be the tailnumber OR the flightnumber! Cannot be longer than 7 chars+nullbyte!
    float x[TARGETS] = { 0 };
    float y[TARGETS] = { 0 };
    float z[TARGETS] = { 0 };
    double alat[TARGETS] = { 0 };
    double alon[TARGETS] = { 0 };
    double aalt[TARGETS] = { 0 };
    float psi[TARGETS] = { 0 };
    int icon[TARGETS] = { 0 };
    int alive[TARGETS] = { 0 };

    void Enable();
    void Disable();
    void SetData(int id,int alive,float x,float y,float z,float psi,float lat,float lon,float alt,int icon);
    void ReceiveMessage(XPLMPluginID from, int msg, void* param);
};

TCASAPI* getTCASAPI();