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
#include <string.h>
#include <math.h>
#include <stdio.h>
//#include "vec_opps.h"
#include "XPLMPlanes.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include <string>
#include "XPLMDisplay.h"
#include "XPLMScenery.h"
#include "XPLMInstance.h"
#include "XPLMPlugin.h"
/*#if defined(__APPLE__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif defined(_WIN64)
#include <al.h>
#include <alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif*/
#include <time.h>
#include "XPLMCamera.h"
#include "jvm.h"
#include "Simulation.h"
 #include "aiplane.h"
#include "AISound.h"
#define PI 3.14159265

WaveFile::WaveFile(){

}
char * WaveFile::find_chunk(char * file_begin, char * file_end, int desired_id, int swapped)
{
	while(file_begin < file_end)
	{
		chunk_header * h = (chunk_header *) file_begin;
		if(h->id == desired_id && !swapped)
			return file_begin+sizeof(chunk_header);
		if(h->id == SWAP_32(desired_id) && swapped)
			return file_begin+sizeof(chunk_header);
		int chunk_size = swapped ? SWAP_32(h->size) : h->size;
		char * next = file_begin + chunk_size + sizeof(chunk_header);
		if(next > file_end || next <= file_begin)
			return NULL;
		file_begin = next;		
	}
	return NULL;
}
char * WaveFile::chunk_end(char * chunk_start, int swapped)
{
	chunk_header * h = (chunk_header *) (chunk_start - sizeof(chunk_header));
	return chunk_start + (swapped ? SWAP_32(h->size) : h->size);
}
int sound_id=0;
ALuint buffers[6];
ALuint snd_srcs[8];

ALuint WaveFile::load_wave(const char * file_name,int thisID)
{
	// First: we open the file and copy it into a single large memory buffer for processing.

	FILE * fi = fopen(file_name,"rb");
	if(fi == NULL)
	{
		char error[2056];
		sprintf(error,"AutoATC: WAVE file load failed - could not open:%s\n",file_name);
		XPLMDebugString(error);	
		return 0;
	}
	fseek(fi,0,SEEK_END);
	int file_size = (int)ftell(fi);
	fseek(fi,0,SEEK_SET);
	char * mem = (char*) malloc(file_size);
	if(mem == NULL)
	{
		XPLMDebugString("AutoATC: WAVE file load failed - could not allocate memory.\n");
		fclose(fi);
		return 0;
	}
	if (fread(mem, 1, file_size, fi) != file_size)
	{
		XPLMDebugString("AutoATC: WAVE file load failed - could not read file.\n");	
		free(mem);
		fclose(fi);
		return 0;
	}
	fclose(fi);
	char * mem_end = mem + file_size;
	
	// Second: find the RIFF chunk.  Note that by searching for RIFF both normal
	// and reversed, we can automatically determine the endian swap situation for
	// this file regardless of what machine we are on.
	
	int swapped = 0;
	char * riff = find_chunk(mem, mem_end, RIFF_ID, 0);
	if(riff == NULL)
	{
		riff = find_chunk(mem, mem_end, RIFF_ID, 1);
		if(riff)
			swapped = 1;
		else{
			XPLMDebugString("Could not find RIFF chunk in wave file.\n"); 
			free(mem);
		}
			//FAIL("Could not find RIFF chunk in wave file.\n")
	}
	
	// The wave chunk isn't really a chunk at all. :-(  It's just a "WAVE" tag 
	// followed by more chunks.  This strikes me as totally inconsistent, but
	// anyway, confirm the WAVE ID and move on.
	
	if (riff[0] != 'W' ||
		riff[1] != 'A' ||
		riff[2] != 'V' ||
		riff[3] != 'E')
		{
			XPLMDebugString("Could not find WAVE signature in wave file.\n"); 
			free(mem);
		}
			//FAIL("Could not find WAVE signature in wave file.\n")

	char * format = find_chunk(riff+4, chunk_end(riff,swapped), FMT_ID, swapped);
	if(format == NULL)
		{
			XPLMDebugString("Could not find FMT  chunk in wave file.\n"); 
			free(mem);
		}
			//FAIL("Could not find FMT  chunk in wave file.\n")
	
	// Find the format chunk, and swap the values if needed.  This gives us our real format.
	
	format_info * fmt = (format_info *) format;
	if(swapped)
	{
		fmt->format = SWAP_16(fmt->format);
		fmt->num_channels = SWAP_16(fmt->num_channels);
		fmt->sample_rate = SWAP_32(fmt->sample_rate);
		fmt->byte_rate = SWAP_32(fmt->byte_rate);
		fmt->block_align = SWAP_16(fmt->block_align);
		fmt->bits_per_sample = SWAP_16(fmt->bits_per_sample);
	}
	
	// Reject things we don't understand...expand this code to support weirder audio formats.
	
	if(fmt->format != 1){
			XPLMDebugString("Wave file is not PCM format data.\n"); 
			free(mem);
		}
			// FAIL("Wave file is not PCM format data.\n")
	if(fmt->num_channels != 1 && fmt->num_channels != 2){
			XPLMDebugString("Must have mono or stereo sound.\n"); 
			free(mem);
		}
			// FAIL("Must have mono or stereo sound.\n")
	if(fmt->bits_per_sample != 8 && fmt->bits_per_sample != 16){
			XPLMDebugString("Must have 8 or 16 bit sounds.\n"); 
			free(mem);
		}
			// FAIL("Must have 8 or 16 bit sounds.\n")
	char * data = find_chunk(riff+4, chunk_end(riff,swapped), DATA_ID, swapped) ;
	if(data == NULL)
		{
			XPLMDebugString("I could not find the DATA chunk.\n"); 
			free(mem);
		}
			//FAIL("I could not find the DATA chunk.\n")
	
	int sample_size = fmt->num_channels * fmt->bits_per_sample / 8;
	int data_bytes = (int)(chunk_end(data,swapped) - data);
	int data_samples = data_bytes / sample_size;
	
	// If the file is swapped and we have 16-bit audio, we need to endian-swap the audio too or we'll 
	// get something that sounds just astoundingly bad!
	
	if(fmt->bits_per_sample == 16 && swapped)
	{
		short * ptr = (short *) data;
		int words = data_samples * fmt->num_channels;
		while(words--)
		{
			*ptr = SWAP_16(*ptr);
			++ptr;
		}
	}
	
	// Finally, the OpenAL crud.  Build a new OpenAL buffer and send the data to OpenAL, passing in
	// OpenAL format enums based on the format chunk.
	
	//ALuint buf_id = buffers[sound_id];
	//CHECK_ERR();
	//if(thisID==0)
		
	//CHECK_ERR();
	if(buffers[thisID] == 0){
			XPLMDebugString("Could not generate buffer id.\n"); 
			free(mem);
			return -1;
		}
			//("Could not generate buffer id.\n");
	
	/*alBufferData(buffers[thisID], fmt->bits_per_sample == 16 ? 
							(fmt->num_channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16) :
							(fmt->num_channels == 2 ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8),
					data, data_bytes, fmt->sample_rate);*/
	free(mem);
    //snd_buffer=
	char error[2056];
	sprintf(error,"AutoATC: WAVE file loaded:%s\n",file_name);
	XPLMDebugString(error);	
	return buffers[thisID];
}
//WaveFile buffers[2];
WaveFile wav;
void AircraftSound::LoadSound()
{
	/*ALfloat	zero[3] = { 0 } ;

	CHECK_ERR();
	float looping=1.0f;
	char path[1024];
	if(local_snd_id==0){
		looping=0.0f;
		snd_src=snd_srcs[local_snd_id];
		sprintf (path, "%s",  "Resources/plugins/AutoATC/audio/screech.wav");
		snd_buffer=wav.load_wave(path,local_snd_id);
		
	}
	else if(local_snd_id==1){
		
		snd_src=snd_srcs[local_snd_id];
		sprintf (path, "%s",  "Resources/plugins/AutoATC/audio/prop.wav");
		snd_buffer=wav.load_wave(path,local_snd_id);
		
	}
	else if(local_snd_id==2){
		snd_src=snd_srcs[local_snd_id];
		sprintf (path, "%s",  "Resources/plugins/AutoATC/audio/heli2.wav");//heli
		snd_buffer=wav.load_wave(path,local_snd_id);
		
	}
	else if(local_snd_id==3){
		snd_src=snd_srcs[local_snd_id];
		sprintf (path, "%s",  "Resources/plugins/AutoATC/audio/tprop.wav");
		snd_buffer=wav.load_wave(path,local_snd_id);
		
	}
	else if(local_snd_id==4){
		snd_src=snd_srcs[local_snd_id];
		sprintf (path, "%s",  "Resources/plugins/AutoATC/audio/jet.wav");
		snd_buffer=wav.load_wave(path,local_snd_id);
		
	}
	else{
		if(!snd_srcs[local_snd_id]){
			sprintf (path, "%s",  "Resources/plugins/AutoATC/audio/jet.wav");
			snd_buffer=wav.load_wave(path,4);
		}
		snd_src=snd_srcs[local_snd_id];
		snd_buffer=buffers[4];
		
	}
	
	CHECK_ERR();
	
	// Basic initializtion code to play a sound: specify the buffer the source is playing, as well as some 
	// sound parameters. This doesn't play the sound - it's just one-time initialization.
	alSourcei(snd_src,AL_BUFFER,snd_buffer);
	alSourcef(snd_src,AL_PITCH,1.0f);
	alSourcef(snd_src,AL_GAIN,1.0f);	
	alSourcei(snd_src,AL_LOOPING,looping);
	alSourcei(snd_src,AL_SOURCE_RELATIVE, AL_FALSE);
	alSourcefv(snd_src,AL_VELOCITY, zero);
    //zero[0]=2.0f;
    zero[2]=5.0f;
    alSource3f(snd_src,AL_POSITION, zero[0],zero[1],zero[2]);
	printf("AutoATC:load sound %d\n",local_snd_id);
	CHECK_ERR();*/
}
AircraftSound::AircraftSound()
{
   // WaveFile wav;
    local_snd_id=sound_id;
	sound_id++;
}
void AircraftSound::play(){
	/*if(!snd_src)
		LoadSound();
    if(snd_src)
		{
            CHECK_ERR();
            
            alSourcef(snd_src,AL_PITCH,pitch);
			alSourcePlay(snd_src);
            CHECK_ERR();
        }*/
}
void AircraftSound::pause(){
   /* if(snd_src)
		{
            //alSourcef(snd_src,AL_PITCH,pitch);
			alSourceStop(snd_src);
			alSourceRewind(snd_src);
        }
    paused=true;
    playing=false;*/
}
void AircraftSound::setPosition(ALfloat pos[]){
	/*JVM* jvmO=getJVM();
    float nowT=jvmO->getSysTime();//clock()/(CLOCKS_PER_SEC*1.0f);
    if(nowT<(startPitchChange+5)){
        double achange=targetPitch-pitch;
        double change=achange*((nowT-startPitchChange)/5.0f);
        
        float thisPitch=(pitch+change);
        //printf("setPitch %f %f %f %f\n",achange,change,thisPitch,(nowT-startPitchChange));
        alSourcef(snd_src,AL_PITCH,thisPitch);
    }
    else
        alSourcef(snd_src,AL_PITCH,targetPitch);
    alSource3f(snd_src,AL_POSITION, pos[0],pos[1],pos[2]);*/
}
void AircraftSound::setVelocity(ALfloat pos[]){
    
    //alSource3f(snd_src,AL_VELOCITY, pos[0],pos[1],pos[2]);
}


// Simple mixer to control AI aircraft volume
void AircraftSound::setVolume(float sound_vol){
	
	//alSourcef(snd_src,AL_GAIN,sound_vol);
}
void AircraftSound::stop(){
   // if(snd_src)		alDeleteSources(1,&snd_src);
   // if(snd_buffer) alDeleteBuffers(1,&snd_buffer);
	//if(buffers[0].snd_buffer) alDeleteBuffers(1,&buffers[0].snd_buffer);
   // if(buffers[1].snd_buffer) alDeleteBuffers(1,&buffers[1].snd_buffer);
}
void AircraftSound::setPitch(float setpitch){
    /*if(setpitch==targetPitch)
        return;
    //pitch=targetPitch;
	JVM* jvmO=getJVM();
	float nowT=jvmO->getSysTime();//clock()/(CLOCKS_PER_SEC*1.0f);
	if(nowT<(startPitchChange+5)){
        double achange=targetPitch-pitch;
        double change=achange*((nowT-startPitchChange)/5.0f);
        
        pitch=(pitch+change);
        
    }
    else
        pitch=targetPitch;
    startPitchChange=jvmO->getSysTime();//clock()/CLOCKS_PER_SEC*1.0f; 
    targetPitch= setpitch;  */
}
AircraftSounds::AircraftSounds(Aircraft *aircraft)
{
    aircrafts=aircraft;
	
    //start();
    
}
void AircraftSounds::showActive(){
   /* if(!live){
        start();

		alGenSources(6,snd_srcs);
		alGenBuffers(5, buffers);
		ALfloat	zero[3] = { 0,0,0 } ;
    	ALfloat	listenerOri[]={0.0,0.0,-1.0, 0.0,1.0,0.0};	// Listener facing into the screen
	
    	alListenerfv(AL_POSITION,zero);
    	alListenerfv(AL_VELOCITY,zero);
    	alListenerfv(AL_ORIENTATION,listenerOri); 	// Orientation ...
    	XPLMDebugString("AutoATC:OpenAL device active.\n");
	}*/


}
void AircraftSounds::land(int aircraftid){
	/* v apos = aircrafts[aircraftid].getSndSrc();
    landsnd.dist=300.0f; // Was 3000 m
    landsnd.pos=v(0,0,0);
    landsnd.velocity=v(0,0,0);
	XPLMCameraPosition_t camera;
    XPLMReadCameraPosition(&camera);
       v s(camera.x,camera.z,camera.y);
       
        v diff=apos-s;
        
        float dist=(diff/diff);//get the magnitude
        if(dist<landsnd.dist){
            landsnd.pos=diff;
            landsnd.dist=dist;
           // landsnd.velocity= aircrafts[i].getVelocity();
			landsnd.pause();
			ALfloat	pos[3] = { landsnd.pos.x/50.0f,landsnd.pos.y/50.0f,landsnd.pos.z/50.0f } ;
			landsnd.setPosition(pos);
            landsnd.setVolume(sound_vol);
            landsnd.play();
            landsnd.paused=true;
        }*/
}
void AircraftSounds::update(float latest_sound_vol){
    
    /*XPLMCameraPosition_t camera;
    XPLMReadCameraPosition(&camera);
    ALfloat	zero[3] = { 0,0,0 } ;//sin(camera.heading*PI/180.0)
    ALfloat	listenerOri[]={(ALfloat)sin(camera.heading*PI/180.0f),0.0f,(ALfloat)cos(camera.heading*PI/180.0f), 0.0f,-1.0f,0.0f};	// Listener facing into the screen
    alListenerfv(AL_POSITION,zero);
    alListenerfv(AL_VELOCITY,zero);
    alListenerfv(AL_ORIENTATION,listenerOri); 	// Orientation ...
    if(sound_vol != latest_sound_vol){
#if defined(DEBUG_STRINGS)
		char message[128];
		//sprintf(message,"AutoATC: XP Sound: Slider Master/Exterior = %f/%f, View external = %i, Door/canopy open = %i/%f\n",slider_master,slider_exterior,viewisexternal,dooropen,canopyopen);
		//XPLMDebugString(message);
		//printf(message);
		sprintf(message,"AutoATC: AI Sound Volume Change: %f -> %f\n",sound_vol,latest_sound_vol);
		XPLMDebugString(message);
		printf(message);
		
#endif		
		sound_vol=latest_sound_vol;
	}
	for(int i=0;i<2;i++){
    	jetsnd[i].dist=3000.0f;
    	jetsnd[i].pos=v(0,0,0);
   		jetsnd[i].velocity=v(0,0,0);
	}
    propsnd.dist=3000.0f;
    propsnd.pos=v(0,0,0);
    propsnd.velocity=v(0,0,0);
	tpropsnd.dist=3000.0f;
    tpropsnd.pos=v(0,0,0);
    tpropsnd.velocity=v(0,0,0);
	helisnd.dist=3000.0f;
    helisnd.pos=v(0,0,0);
    helisnd.velocity=v(0,0,0);
    for(int i=0;i<30;i++){
        v apos = aircrafts[i].getSndSrc();
        int soundIndex= aircrafts[i].soundIndex;
       v s(camera.x,camera.z,camera.y);
       
        v diff=apos-s;
        
        float dist=(diff/diff);//get the magnitude
		if(aircrafts[i].data.engineoff)
			dist=6000.0f;
        if(soundIndex==0){
			for(int n=0;n<2;n++){
				if(dist<jetsnd[n].dist){
					jetsnd[n].aircraftid=i;
           			jetsnd[n].pos=diff;
            		jetsnd[n].dist=dist;
            		jetsnd[n].velocity= aircrafts[i].getVelocity();
					break;
				}
			}
        }
        else if(soundIndex==1&&dist<propsnd.dist){
			propsnd.aircraftid=i;
            propsnd.pos=diff;
            propsnd.dist=dist;
            propsnd.velocity= aircrafts[i].getVelocity();
            
        }
        else if(soundIndex==2&&dist<helisnd.dist){
			helisnd.aircraftid=i;
            helisnd.pos=diff;
            helisnd.dist=dist;
            helisnd.velocity= aircrafts[i].getVelocity();
            
        }
		else if(soundIndex==3&&dist<tpropsnd.dist){
			tpropsnd.aircraftid=i;
            tpropsnd.pos=diff;
            tpropsnd.dist=dist;
            tpropsnd.velocity= aircrafts[i].getVelocity();
            
        }
        
    }
	//
	for(int n=0;n<2;n++){
    if(jetsnd[n].dist<3000.0f&&aircrafts[jetsnd[n].aircraftid].airFrameIndex>=0){
        //float	*apos = aircrafts[closest].getSndSrc();
      if(jetsnd[n].lastAFID!=jetsnd[n].aircraftid){
#if defined(DEBUG_STRINGS)
		  char debugStr[512];
		   sprintf(debugStr,"got plane snd1 af=%d index=%d %f %f %f %f %f %f %d %d\n",jetsnd[n].aircraftid,aircrafts[jetsnd[n].aircraftid].airFrameIndex,jetsnd[n].pos.x,jetsnd[n].pos.y,jetsnd[n].pos.z, jetsnd[n].velocity.x, jetsnd[n].velocity.y, jetsnd[n].velocity.z,jetsnd[n].snd_src,jetsnd[n].snd_buffer);
		  	XPLMDebugString(debugStr);
#endif
		   jetsnd[n].lastAFID=jetsnd[n].aircraftid;
	   }
        ALfloat	pos[3] = { jetsnd[n].pos.x/50.0f,jetsnd[n].pos.y/50.0f,jetsnd[n].pos.z/50.0f } ;
        ALfloat	vel[3] = { jetsnd[n].velocity.x/50.0f,jetsnd[n].velocity.z/50.0f,jetsnd[n].velocity.y/50.0f } ;
		
       // pos[2]=9.0f;
       if(!jetsnd[n].playing){
                jetsnd[n].playing=true;
                // printf("got planejetsnd[n] %f %f %f %f %f %f %d %d\n",jetsnd[n].pos.x,jetsnd[n].pos.y,jetsnd[n].pos.z, jetsnd[n].velocity.x, jetsnd[n].velocity.y, jetsnd[n].velocity.z,snd2.snd_src,snd2.snd_buffer);
                jetsnd[n].play();
                jetsnd[n].paused=false;
            }
            if((jetsnd[n].velocity/jetsnd[n].velocity)>0.0f){
				//if(jetsnd[n].targetPitch!=2.0f)
				//	land(jetsnd[n].aircraftid);
                jetsnd[n].setPitch(2.0f);
            }
            else
                jetsnd[n].setPitch(0.8f);
         jetsnd[n].setPosition(pos);
        jetsnd[n].setVelocity(vel);
        jetsnd[n].setVolume(sound_vol);
       // printf("set listener %f %f %f\n",apos[0],apos[1],apos[2]);
    }
    else if(!jetsnd[n].paused){
            jetsnd[n].pause();
            
        }
	}
    if(propsnd.dist<3000.0f&&aircrafts[propsnd.aircraftid].airFrameIndex>=0){
        //float	*apos = aircrafts[closest].getSndSrc();
       // printf("got plane %f %f %f %f %f %f\n",closest.x,closest.y,closest.z,velocity.x,velocity.y,velocity.z);
       if(propsnd.lastAFID!=propsnd.aircraftid){
#if defined(DEBUG_STRINGS)
			char debugStr[512];
		   sprintf(debugStr,"got plane snd2 af=%d index=%d %f %f %f %f %f %f %d %d\n",propsnd.aircraftid,aircrafts[propsnd.aircraftid].airFrameIndex,propsnd.pos.x,propsnd.pos.y,propsnd.pos.z, propsnd.velocity.x, propsnd.velocity.y, propsnd.velocity.z,propsnd.snd_src,propsnd.snd_buffer);
		   XPLMDebugString(debugStr);
#endif
		   propsnd.lastAFID=propsnd.aircraftid;
	   }
        ALfloat	pos[3] = { propsnd.pos.x/50.0f,propsnd.pos.y/50.0f,propsnd.pos.z/50.0f } ;
        ALfloat	vel[3] = { propsnd.velocity.x/50.0f,propsnd.velocity.z/50.0f,propsnd.velocity.y/50.0f } ;

       // pos[2]=9.0f;
       if(!propsnd.playing){
		   //printf("got plane 2 %f %f %f %f %f %f %d %d\n",snd2.pos.x,snd2.pos.y,snd2.pos.z, snd2.velocity.x, snd2.velocity.y, snd2.velocity.z,snd2.snd_src,snd2.snd_buffer);
                propsnd.playing=true;
                propsnd.play();
                propsnd.paused=false;
            }
            if(propsnd.velocity/propsnd.velocity>0.1f){
                propsnd.setPitch(1.0f);

            }
            else
                propsnd.setPitch(0.7f);
         propsnd.setPosition(pos);
        propsnd.setVelocity(vel);
        propsnd.setVolume(sound_vol);
       // printf("set listener %f %f %f\n",apos[0],apos[1],apos[2]);
    }
    else if(!propsnd.paused){
            propsnd.pause();
            
    }
	if(tpropsnd.dist<3000.0f&&aircrafts[tpropsnd.aircraftid].airFrameIndex>=0){
        //float	*apos = aircrafts[closest].getSndSrc();
       // printf("got plane %f %f %f %f %f %f\n",closest.x,closest.y,closest.z,velocity.x,velocity.y,velocity.z);
       if(tpropsnd.lastAFID!=tpropsnd.aircraftid){
#if defined(DEBUG_STRINGS)
			char debugStr[512];
		   sprintf(debugStr,"got plane snd2 af=%d index=%d %f %f %f %f %f %f %d %d\n",tpropsnd.aircraftid,aircrafts[tpropsnd.aircraftid].airFrameIndex,tpropsnd.pos.x,tpropsnd.pos.y,tpropsnd.pos.z, tpropsnd.velocity.x, tpropsnd.velocity.y, tpropsnd.velocity.z,tpropsnd.snd_src,tpropsnd.snd_buffer);
		   XPLMDebugString(debugStr);
#endif
		   tpropsnd.lastAFID=tpropsnd.aircraftid;
	   }
        ALfloat	pos[3] = { tpropsnd.pos.x/50.0f,tpropsnd.pos.y/50.0f,tpropsnd.pos.z/50.0f } ;
        ALfloat	vel[3] = { tpropsnd.velocity.x/50.0f,tpropsnd.velocity.z/50.0f,tpropsnd.velocity.y/50.0f } ;

       // pos[2]=9.0f;
       if(!tpropsnd.playing){
		   //printf("got plane 2 %f %f %f %f %f %f %d %d\n",snd2.pos.x,snd2.pos.y,snd2.pos.z, snd2.velocity.x, snd2.velocity.y, snd2.velocity.z,snd2.snd_src,snd2.snd_buffer);
                tpropsnd.playing=true;
                tpropsnd.play();
                tpropsnd.paused=false;
            }
            if(tpropsnd.velocity/tpropsnd.velocity>0.1f){
                tpropsnd.setPitch(1.0f);

            }
            else
                tpropsnd.setPitch(0.7f);
         tpropsnd.setPosition(pos);
        tpropsnd.setVelocity(vel);
        tpropsnd.setVolume(sound_vol);
       // printf("set listener %f %f %f\n",apos[0],apos[1],apos[2]);
    }
    else if(!tpropsnd.paused){
            tpropsnd.pause();
            
    }
   if(helisnd.dist<3000.0f&&aircrafts[helisnd.aircraftid].airFrameIndex>=0){
        //float	*apos = aircrafts[closest].getSndSrc();
       // printf("got plane %f %f %f %f %f %f\n",closest.x,closest.y,closest.z,velocity.x,velocity.y,velocity.z);
       if(helisnd.lastAFID!=helisnd.aircraftid){
#if defined(DEBUG_STRINGS)
			char debugStr[512];
		   sprintf(debugStr,"got plane snd3 af=%d index=%d %f %f %f %f %f %f %d %d\n",helisnd.aircraftid,aircrafts[helisnd.aircraftid].airFrameIndex,helisnd.pos.x,helisnd.pos.y,helisnd.pos.z, helisnd.velocity.x, helisnd.velocity.y, helisnd.velocity.z,helisnd.snd_src,helisnd.snd_buffer);
		   XPLMDebugString(debugStr);
#endif
		   helisnd.lastAFID=helisnd.aircraftid;
	   }
        ALfloat	pos[3] = { helisnd.pos.x/50.0f,helisnd.pos.y/50.0f,helisnd.pos.z/50.0f } ;
        ALfloat	vel[3] = { helisnd.velocity.x/50.0f,helisnd.velocity.z/50.0f,helisnd.velocity.y/50.0f } ;

       // pos[2]=9.0f;
       if(!helisnd.playing){
		   //printf("got plane 2 %f %f %f %f %f %f %d %d\n",snd2.pos.x,snd2.pos.y,snd2.pos.z, snd2.velocity.x, snd2.velocity.y, snd2.velocity.z,snd2.snd_src,snd2.snd_buffer);
                helisnd.playing=true;
                helisnd.play();
                helisnd.paused=false;
            }
            if(helisnd.velocity/propsnd.velocity>0.1f){
                propsnd.setPitch(0.85f);

            }
            else
                propsnd.setPitch(0.8f);
        helisnd.setPosition(pos);
        helisnd.setVelocity(vel);
        helisnd.setVolume(sound_vol);
       // printf("set listener %f %f %f\n",apos[0],apos[1],apos[2]);
    }
    else if(!helisnd.paused){
            helisnd.pause();
            
    }*/
    
}
void AircraftSounds::start()
{
   /* if(live)
        return;
    live=false;
    CHECK_ERR();

	//char buf[2048];
	
	// We have to save the old context and restore it later, so that we don't interfere with X-Plane
	// and other plugins.

	ALCcontext * old_ctx =alcGetCurrentContext();
	
	if(old_ctx == NULL)
	{
		printf("AutoATC:0x%08x: I found OpenAL, I will init.\n",XPLMGetMyID());
		my_dev = alcOpenDevice(NULL);
		if(my_dev == NULL)
		{
			XPLMDebugString("AutoATC:Could not open the default OpenAL device.\n");
			return;		
		}	
		my_ctx = alcCreateContext(my_dev, NULL);
		if(my_ctx == NULL)
		{
			if(old_ctx)
				alcMakeContextCurrent(old_ctx);
			alcCloseDevice(my_dev);
			my_dev = NULL;
			XPLMDebugString("AutoATC: Could not create a context.\n");
			return;				
		}
		else
		{
			createdContext=true;
		}
		
		
		// Make our context current, so that OpenAL commands affect our, um, stuff.
		
		alcMakeContextCurrent(my_ctx);
		printf("0x%08x: I created the context.\n",XPLMGetMyID());

		ALCint		major_version, minor_version;
		const char * al_hw=alcGetString(my_dev,ALC_DEVICE_SPECIFIER	);
		const char * al_ex=alcGetString(my_dev,ALC_EXTENSIONS		
);
		alcGetIntegerv(NULL,ALC_MAJOR_VERSION,sizeof(major_version),&major_version);
		alcGetIntegerv(NULL,ALC_MINOR_VERSION,sizeof(minor_version),&minor_version);
		
		printf("AutoATC:OpenAL version   : %d.%d\n",major_version,minor_version);
		printf("AutoATC:OpenAL hardware  : %s\n", (al_hw?al_hw:"(none)"));
		printf("AutoATC:OpenAL extensions: %s\n", (al_ex?al_ex:"(none)"));
		CHECK_ERR();

	} 
	else
	{
		printf("AutoATC: I found someone else's openAL context.\n");
		old_ctx=old_ctx;
	}
	
	live=true;
	XPLMDebugString("AutoATC:Using OpenAL device.\n");
	
    //buffers[0].load_wave("/media/storage2/X-Plane 11/X-Plane 11_11.30/Resources/plugins/AutoATC/sound4.wav");
	/*CHECK_ERR();
	alGenBuffers(3, buffers);
	CHECK_ERR();*/
	//printf("0x%08x: Loaded %d from %s\n", XPLMGetMyID(), snd_buffer,"/media/storage2/X-Plane 11/X-Plane 11_11.30/Resources/plugins/AutoATC/sound4.wav");
	return;
}
void AircraftSounds::stop()
{
	printf("AutoATC:stopping sound.\n");
	

	/*landsnd.stop();
    propsnd.stop();
    helisnd.stop();
	tpropsnd.stop();
	for(int i=0;i<3;i++)
    	jetsnd[i].stop();
	if(my_ctx != NULL)
		alcDestroyContext(my_ctx);
	if(createdContext){
		
		alcCloseDevice(my_dev);
	}	
	live=false;
    //snd1.stop();
    //snd2.stop();*/
}
