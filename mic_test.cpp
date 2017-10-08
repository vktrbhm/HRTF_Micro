#include <iostream>
#include <cmath>


#include <AL/al.h>    // OpenAL header files
#include <AL/alc.h>

#include <list>

#include "vec3.hpp"



#define FREQ 41000   // Sample rate
#define CAP_SIZE 2048 // How much to capture at a time (affects latency)


Vec3 setSourcePosition( Vec3 & pos) {
    
    pos.x_ = 2 *  cos(pos.phi_) * 180 / M_PI;
    pos.y_ = cos(pos.phi_)* 180 / M_PI;
    pos.z_ = 2 * sin(pos.phi_) * 180 / M_PI ;

    pos.phi_+= 0.01;

    return pos;
}

Vec3 setListenerOrientation(Vec3 & ori) {
    ori.x_ = 3 * cos(ori.phi_);
    ori.z_ = 3 * sin(ori.phi_);
    ori.phi_ += 0.1;
    return ori;
}



int main(int argC,char* argV[])
{
    std::list<ALuint> bufferQueue; // Queue of buffer objects
    Vec3 pos (0.0,0.0,0.0,0.0);
    float phi;
    Vec3 ori;


    ALenum errorCode=0;
    ALuint helloBuffer[16], helloSource[1];
    ALCdevice* audioDevice = alcOpenDevice(nullptr); // Request default audio device
    errorCode = alcGetError(audioDevice);
    ALCcontext* audioContext = alcCreateContext(audioDevice,nullptr); // Create the audio context
    alcMakeContextCurrent(audioContext);
    errorCode = alcGetError(audioDevice);
    // Request the default capture device with a half-second buffer
    ALCdevice* inputDevice = alcCaptureOpenDevice(nullptr,FREQ,AL_FORMAT_MONO16,FREQ/2);
    errorCode = alcGetError(inputDevice);
    alcCaptureStart(inputDevice); // Begin capturing
    errorCode = alcGetError(inputDevice);

    alGenBuffers(16,&helloBuffer[0]); // Create some buffer-objects
    errorCode = alGetError();


    // Queue our buffers onto an STL list
    for (int ii=0;ii<16;++ii) {
        bufferQueue.push_back(helloBuffer[ii]);
    }

    alGenSources (1, &helloSource[0]); // Create a sound source
    errorCode = alGetError();

    short buffer[FREQ*2]; // A buffer to hold captured audio
    ALCint samplesIn=0;  // How many samples are captured
    ALint availBuffers=0; // Buffers to be recovered
    ALuint myBuff; // The buffer we're using
    ALuint buffHolder[16]; // An array to hold catch the unqueued buffers
    bool done = false;
    while (!done) { // Main loop
        // Poll for recoverable buffers
        alGetSourcei(helloSource[0],AL_BUFFERS_PROCESSED,&availBuffers);
        if (availBuffers>0) {
            alSourceUnqueueBuffers(helloSource[0],availBuffers,buffHolder);
            for (int ii=0;ii<availBuffers;++ii) {
                // Push the recovered buffers back on the queue
                bufferQueue.push_back(buffHolder[ii]);
            }
        }

         

        // Poll for captured audio
        alcGetIntegerv(inputDevice,ALC_CAPTURE_SAMPLES,1,&samplesIn);


        if (samplesIn>CAP_SIZE) {
            // Grab the sound
            alcCaptureSamples(inputDevice,buffer,CAP_SIZE);

            // Stuff the captured data in a buffer-object
            if (!bufferQueue.empty()) { // We just drop the data if no buffers are available
                myBuff = bufferQueue.front(); bufferQueue.pop_front();
                alBufferData(myBuff,AL_FORMAT_MONO16,buffer,CAP_SIZE*sizeof(short),FREQ);

               

                // Source Position                
                setSourcePosition(pos);
                
                alSourceQueueBuffers(helloSource[0],1,&myBuff);
                alSource3f(helloSource[0], AL_POSITION, pos.x_ , pos.y_, pos.z_);
                alSourcef (helloSource[0], AL_GAIN, 20.0);

                std::cout<< "Position : ( x=" << pos.x_<<", y= "<<pos.y_<< " z= "<<pos.z_<<") \n";
                std::cout<< "Angle : (" << pos.phi_<<") \n";


                ALint sState=0;
                alGetSourcei(helloSource[0],AL_SOURCE_STATE,&sState);



                if (sState!=AL_PLAYING) {
                    alSourcePlay(helloSource[0]);
                }
            }
        }
    }
    // Stop capture
    alcCaptureStop(inputDevice);
    alcCaptureCloseDevice(inputDevice);

    // Stop the sources
    alSourceStopv(1,&helloSource[0]);
    for (int ii=0;ii<1;++ii) {
        alSourcei(helloSource[ii],AL_BUFFER,0);
    }
    // Clean-up 
    alDeleteSources(1,&helloSource[0]); 
    alDeleteBuffers(16,&helloBuffer[0]);
    errorCode = alGetError();
    alcMakeContextCurrent(nullptr);
    errorCode = alGetError();
    alcDestroyContext(audioContext);
    alcCloseDevice(audioDevice);

    return 0;
}