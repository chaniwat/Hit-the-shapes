#include "sound.h"

ALvoid Sound::genSoundFromFile(const char* filename)
{
    ALenum error;

    /* Create an AL buffer from the given sound file. */
    this->sampingSet = alutCreateBufferFromFile(filename);

    /* Generate a single source, attach the buffer to it and start playing. */
    alGenSources(1, &this->Source);
    alSourcei(this->Source, AL_BUFFER, this->sampingSet);
}

ALvoid Sound::PlayLoop()
{
    if (!this->isPlaying)
    {
        alSourcei(this->Source, AL_LOOPING, AL_TRUE);
        alSourcePlay(this->Source);
        this->isPlaying = AL_TRUE;
    }
}

ALvoid Sound::PlayOnce()
{
    if (!this->isPlaying)
    {
        alSourcei(this->Source, AL_LOOPING, AL_FALSE);
        alSourcePlay(this->Source);
        this->isPlaying = AL_TRUE;
    }
}

ALvoid Sound::Stop()
{
    if (this->isPlaying)
    {
        alSourceStop(this->Source);
        this->isPlaying = AL_FALSE;
    }
}