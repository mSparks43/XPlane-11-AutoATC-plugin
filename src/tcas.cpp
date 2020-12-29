#include "tcas.h"
#include <stdio.h>
#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"
#include "XPLMPlanes.h"
#include "XPLMUtilities.h"
#include "XPLMPlugin.h"
static TCASAPI tcasAPI=TCASAPI();

float floop_cb(float elapsed1, float elapsed2, int ctr, void* refcon)
{
   // static float relbrgs[TARGETS];
    //static float relalts[TARGETS];

    // make some targets change altitude
    //tcasAPI.absalts[0] += 400 * elapsed1 / 60.f;                            // target is climbing 400fpm
    //tcasAPI.absalts[3] -= 600 * elapsed1 / 60.f;                            // target descending 600fpm
    if (tcasAPI.plugin_owns_tcas)
    {
        for (int i = 1; i < TARGETS; ++i)
        {
        // targets are just flying perfect circles around the user.
        /*tcasAPI.absbrgs[i] += (i+1) * elapsed1;                             // this just makes the targets fly circles of varying speed
        relbrgs[i] = tcasAPI.absbrgs[i] - XPLMGetDataf(tcasAPI.psi);                // convert to relative position for TCAS dataref. Use true_psi, not hpath or something else

        relalts[i] = tcasAPI.absalts[i] * fttomtr - XPLMGetDatad(tcasAPI.ele);      // convert to relative position for TCAS dataref. Use elevation, not local_y!*/
            
            if(tcasAPI.alive[i]>0){
                XPLMSetDatavi(tcasAPI.id, &i, i, 1);//always write the id
                XPLMSetDatavf(tcasAPI.xRef, &tcasAPI.x[i], i, 1);
                XPLMSetDatavf(tcasAPI.yRef, &tcasAPI.y[i], i, 1);
                XPLMSetDatavf(tcasAPI.zRef, &tcasAPI.z[i], i, 1);
                XPLMSetDatavf(tcasAPI.psiRef, &tcasAPI.psi[i], i, 1);
                //printf("Setting %d to %f %f %f\n",i,tcasAPI.x[i],tcasAPI.y[i],tcasAPI.z[i]);
            }
        }

    // if we are in charge, we can write four targets to the four TCAS datarefs, starting at index 1
    // Note this dataref write would do nothing if we hadn't acquired the planes and set override_TCAS
    
        // These relative coordinates, or the absolute x/y/z double coordinates must be updated to keep the target flying, obviously.
        // X-Plane will forget about your target if you don't update it for 10 consecutive frames.
        
        // You could also update sim/cockpit2/tcas/targets/position/double/plane1_x, plane1_y, etc.. In which case X-Plane would update the relative bearings for you
        // So for one target, you can write either absolute coorindates or relative bearings, but not both!
        // For mulitple targets, you can update some targets in relative mode, and others in absolute mode. 
    }

    // be sure to be called every frame. A target not updated for 10 successive frames will be dropped.
    return -1;
}


// A simple reset we will call every minute to reset the targets to their initial position and altitude
/*float reset_cb(float elapsed1, float elapsed2, int ctr, void* refcon)
{
    tcasAPI.absbrgs[0] = 0;
    tcasAPI.absbrgs[1] = 90;
    tcasAPI.absbrgs[2] = 180;
    tcasAPI.absbrgs[3] = 270;

    tcasAPI.absalts[0] = 1000;
    tcasAPI.absalts[3] = 4000;
    return 60;                      // call me again in a minute
}*/


// we call this function when we succesfully acquired the AI planes.
// Arthur says: BRILLIANT! My Jets Now!
void my_planes_now()
{
    XPLMDebugString("TCAS test plugin now has the AI planes!\n");
    XPLMSetDatai(tcasAPI.override, 1);      // If you try to set this dataref when not owning the planes, it will fail!
    XPLMSetDatai(XPLMFindDataRef("sim/operation/override/override_multiplayer_map_layer"),1); //disable lame broken xplane map layer (ffs)
    // query the array size. This might change with X-Plane updates.
    std::size_t max_targets = XPLMGetDatavi(tcasAPI.id, NULL, 0, 0);
    //assert(TARGETS < max_targets);

    XPLMSetActiveAircraftCount(TARGETS+1);  // This will give you four targets, even if the user's AI plane count is set to 0. This can be as high as 63!
    tcasAPI.plugin_owns_tcas = true;


    // As long as you keep updating the positions, X-Plane will remember the ID. 
    // These IDs can be used by other plugins to keep track of your aircraft if you shuffle slots. 
    // Note that the ID cannot be left 0! X-Plane will not update your target's dependent datarefs if it has no ID!!
    // If you haven't updated a target for 10 frames, X-Plane will forget it and reset the ID of the slot to 0.
    //XPLMSetDatavi(tcasAPI.id, tcasAPI.ids, 1, TARGETS);


    // Each target can have a 7 ASCII character flight ID, usually the tailnumber or flightnumber
    // it consists of an 8 byte character array, which is null terminated.
    // The array is 64*8 bytes long, and the first 8 bytes are the user's tailnumber obviously.
    // Note that this is, unlike the Mode-S ID, totally optional.
    // But it is nice to see the tainumber on the map obviously!
    int mode=1200;
    for (int i = 1; i <= TARGETS; ++i){
        XPLMSetDatab(tcasAPI.flt_id, tcasAPI.tailnum[i - 1], i * 8, strnlen(tcasAPI.tailnum[i-1], 8));  // copy at most 8 characters, but not more than we actually have.
        XPLMSetDatab(tcasAPI.icaoType, tcasAPI.icaonum[i - 1], i * 8, strnlen(tcasAPI.icaonum[i-1], 8));
        XPLMSetDatavi(tcasAPI.modeC_code,&mode,i,1);
    }


    // start updating
    XPLMRegisterFlightLoopCallback(floop_cb, 1, NULL);
    //XPLMRegisterFlightLoopCallback(reset_cb, 60, NULL);
}

// we call this function when we want to give up on controlling the AI planes
// For example, a network plugin would do this as soon as you disconnect from your multiplayer session!
void someone_elses_planes_now()
{
    // stop updating
    XPLMUnregisterFlightLoopCallback(floop_cb, NULL);
    //XPLMUnregisterFlightLoopCallback(reset_cb, NULL);
    // relinquish control
    tcasAPI.plugin_owns_tcas = false;
    XPLMSetDatai(tcasAPI.override, 0);      // order is important! Relinquish the override first
    XPLMReleasePlanes();            // Then release the AI planes to another plugin! Note that another plugins AcquirePlanes callback function might be called here synchronously
}

// this is a callback that will be called by X-Plane, if we asked for planes, but another plugin had the planes at the time
// but now this other plugin has given up the planes. They essentially yielded control to us. So the planes are up for grabs again!!
void retry_acquiring_planes(void*)
{
    if (!XPLMAcquirePlanes(NULL, &retry_acquiring_planes, NULL))
    {
        // Damn, someone else cut in the queue before us!
        // this can happen if more than two plugins are all competing for AI. 
        XPLMDebugString("TCAS test plugin could not get the AI planes, even after the previous plugin gave them up. We are waiting for the next chance\n");
    }
    else
    {
        // Acquisition succeded.
        my_planes_now();
    }
}
void TCASAPI::SetData(int id,int isAlive,float nx,float ny,float nz,float npsi,float lat,float lon,float alt,int aicon)
{
    //printf("set tcas data %d %d %f %f %f\n",id,isAlive,nx,ny,nz);
    if(isAlive>0){
        alive[id] = 1;
        x[id]=nx;
        y[id]=ny;
        z[id]=nz;
        psi[id]=npsi;
    }
    else{
        alive[id] = 0;
        x[id]=0;
        y[id]=0;
        z[id]=0;
        psi[id]=0;

    }
    alat[id]=lat;
    alon[id]=lon;
    aalt[id]=alt*3.3;
    icon[id]=aicon;
}
void TCASAPI::Enable()
{
    printf("enable tcas");
   // psi = XPLMFindDataRef("sim/flightmodel/position/true_psi");
    //ele = XPLMFindDataRef("sim/flightmodel/position/elevation");

    // these datarefs were read-only until 11.50
    xRef = XPLMFindDataRef("sim/cockpit2/tcas/targets/position/x");
    yRef = XPLMFindDataRef("sim/cockpit2/tcas/targets/position/y");
    zRef = XPLMFindDataRef("sim/cockpit2/tcas/targets/position/z");
    psiRef = XPLMFindDataRef("sim/cockpit2/tcas/targets/position/psi");
    icaoType = XPLMFindDataRef("sim/cockpit2/tcas/targets/icao_type");
    // these datarefs are new to 11.50
    id = XPLMFindDataRef("sim/cockpit2/tcas/targets/modeS_id");         // array of 64 integers
    flt_id = XPLMFindDataRef("sim/cockpit2/tcas/targets/flight_id");    // array of 64*8 bytes
    modeC_code=XPLMFindDataRef("sim/cockpit2/tcas/targets/modeC_code"); 
    // this dataref can only be set if we own the AI planes!
    override = XPLMFindDataRef("sim/operation/override/override_TCAS");


    /// STOP
    /// DROP AND LISTEN
    // In a real application, you would only do the next step if you are immediately ready to supply traffic.
    // I.e. if you are connected to a session if you are a multiplayer plugin. Don't preemptively acquire the traffic
    // just because you might connect to a session some time later!
    // So this piecce of code is probably not going to be in XPluginEnable for you. 
    // It is going to be wherever you are actually done establishing your traffic source!

    // try to acquire planes. If a different plugin has them, this will fail.
    // If the other plugin releases them, our callback will be called.
    if (!XPLMAcquirePlanes(NULL, &retry_acquiring_planes, NULL))
    {
        // If acquisition has failed, gather some intelligence on who currently owns the planes
        int total, active;
        XPLMPluginID controller;
        char who[256];
        XPLMCountAircraft(&total, &active, &controller);
        XPLMGetPluginInfo(controller, who, NULL, NULL, NULL);
        XPLMDebugString("TCAS test plugin could not get the AI planes, because ");
        XPLMDebugString(who);
        XPLMDebugString(" owns the AI planes now. We'll get them when he relinquishes control.\n");
        // Note that the retry callback will be called when this other guy gives up the planes.
    }
    else
    {
        // Acquisition succeded.
        my_planes_now();
    }
}

void TCASAPI::Disable()
{
    printf("disable tcas");
    someone_elses_planes_now();
}

void TCASAPI::ReceiveMessage(XPLMPluginID from, int msg, void* param)
{
    //if (msg == XPLM_MSG_RELEASE_PLANES)
    {
        // Some other plugin wants the AI planes. Since this is just a dummy traffic provider, we yield
        // to a real traffic provider. Deactivate myself now!
        // If this was send to a VATSIM plugin while the user is connected, of course it would just ignore this message.
        someone_elses_planes_now();

        char name[256];
        XPLMGetPluginInfo(from, name, NULL, NULL, NULL);
        XPLMDebugString("TCAS test plugin has given up control of the AI planes to ");
        XPLMDebugString(name);
        XPLMDebugString("\n");
    }
}

TCASAPI* getTCASAPI(){

    return &tcasAPI;
}