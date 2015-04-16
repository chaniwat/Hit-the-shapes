#ifndef SOUND_H
#define SOUND_H

#include <string>

#include <AL\al.h>
#include <AL\alc.h>
#include <AL\alut.h>

class Sound
{
    public:
        // Sound object buffer data
        ALuint Source;
        ALuint sampingSet;
        // Status
        ALboolean isPlaying = AL_FALSE;
        // Generate source sound
        ALvoid genSoundFromFile(const char* filename);
        // Play sound
        ALvoid PlayLoop();
        ALvoid PlayOnce();
        // Stop sound
        ALvoid Stop();
};

#endif SOUND_H