#ifndef SOUNDQUEUE_HPP
#define SOUNDQUEUE_HPP
#include <vector>
#include "Sound.hpp"
#include "al.h" 
#include "alc.h"

class SoundQueue {
public:
	SoundQueue();
	status initialize();
	void finalize();
	void playSound(Sound* pSound);
private:
	ALuint* mBuffer;
	ALuint* mSource;
};
#endif