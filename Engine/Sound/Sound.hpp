#ifndef _PACKT_SOUND_HPP_
#define _PACKT_SOUND_HPP_
#include "al.h"
#include "alc.h"
#include <string>
#include "../Tools/Types.hpp"

class SoundManager;

class Sound {
public:
	Sound(const std::string& pResource);
	const char* getPath() const { return mPath.c_str(); }
	const ALuint* getSource() const { return &mSource; }
	const ALuint* getBuffer() const { return &mBuffer; }
private:
	friend class AudioManager;
	status load();
	status unload();
	std::string mPath;
	ALuint mSource;
	ALuint mBuffer; 

	ALsizei mSize, mFrequency;
	ALenum mFormat;
	ALvoid* mData;
};

#endif