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

#if defined(__APPLE__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif defined(_WIN64)
#include <al.h>
#include <alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif



class AircraftSound
{
    private:
    
    double			pitch		= 1.0f;			// Start with 1.0 pitch - no pitch shift
     		// Start with 1.0 pitch - no pitch shift.
     float startPitchChange;
    public:
    double			targetPitch		= 1.0f;	
    ALuint			snd_src		=0;				// Sample source and buffer - this is one "sound" we play.
    ALuint	snd_buffer	=0;
    int local_snd_id;
   /* float		x;
	float		y;
	float		z;*/
    v pos;
    v velocity;
    int aircraftid;
    int lastAFID;
    float dist;
    bool playing;
    bool paused;
    //v position(0.0,0.0,0.0);
    AircraftSound(void);
    void LoadSound(void);
    void play(void);
    void pause(void);
    void stop();
    void setPitch(float pitch);
    void setPosition(ALfloat pos[]);
    void setVelocity(ALfloat pos[]);

    void setVolume(float volume);
};
class AircraftSounds
{
    private:
    ALCdevice *		my_dev		= NULL;			// We make our own device and context to play sound through.
    ALCcontext *		my_ctx		= NULL;
    bool createdContext=false;
    float sound_vol = 1.0;
    float slider_master = 1.0;
    float slider_exterior = 1.0;
    int viewisexternal = 0;
    float canopyopen = 0.0;
    int dooropen = 0;
    bool live;
    AircraftSound landsnd;
    AircraftSound propsnd;
    AircraftSound helisnd;
    AircraftSound tpropsnd;
    AircraftSound jetsnd[2];

   // AircraftSound landsnd;
    Aircraft *aircrafts;
    
    
    public:
    AircraftSounds(Aircraft aircraft[]);
    void start();
    void stop();
    void showActive();
    void update(float latest_sound_vol);
     void land(int aircraftid);
};
#define SWAP_32(value)                 \
        (((((unsigned short)value)<<8) & 0xFF00)   | \
         ((((unsigned short)value)>>8) & 0x00FF))

#define SWAP_16(value)                     \
        (((((unsigned int)value)<<24) & 0xFF000000)  | \
         ((((unsigned int)value)<< 8) & 0x00FF0000)  | \
         ((((unsigned int)value)>> 8) & 0x0000FF00)  | \
         ((((unsigned int)value)>>24) & 0x000000FF))

//#define FAIL(X) { XPLMDebugString(X); free(mem); return 0; }

#define RIFF_ID 0x46464952			// 'RIFF'
#define FMT_ID  0x20746D66			// 'FMT '
#define DATA_ID 0x61746164			// 'DATA'
#define CHECK_ERR() __CHECK_ERR(__FILE__,__LINE__)

struct chunk_header { 
	int			id;
	int			size;
};

// WAVE file format info.  We pass this through to OpenAL so we can support mono/stereo, 8/16/bit, etc.
struct format_info {
	short		format;				// PCM = 1, not sure what other values are legal.
	short		num_channels;
	int			sample_rate;
	int			byte_rate;
	short		block_align;
	short		bits_per_sample;
};
static bool __CHECK_ERR(const char * f, int l)
{
   /* ALuint e = alGetError();
    if (e != AL_NO_ERROR){
        printf("ERROR: %d (%s:%d\n", e, f, l);
        return true;
    }*/
    return false;
}
#define CHECK_ERR() __CHECK_ERR(__FILE__,__LINE__)

class WaveFile
{
    private:
    char * find_chunk(char * file_begin, char * file_end, int desired_id, int swapped);
    char * chunk_end(char * chunk_start, int swapped);
    public:
    
    WaveFile(void);
    ALuint load_wave(const char * file_name,int thisID);
};
