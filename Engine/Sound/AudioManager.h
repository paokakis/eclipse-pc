#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H
#include <string>
#include <vector>
#include "..\..\sources\glm\glm.hpp"
#include "..\..\Engine\Graphics\GraphicsManager.hpp"
#include "SoundQueue.hpp"
#include "al.h"
#include "alc.h"
#include "Sound.hpp"

#define NUM_BUFFERS 4
#define NUM_SOURCES 4
#define BUFFER_SIZE 4096

class AudioManager
{
public:
	AudioManager(GraphicsManager& graphMan);
	~AudioManager() {}

	status start(const glm::vec3& worldDimens);
	void stop();
	status setListener(glm::vec3& pos, glm::vec3& velocity, glm::vec3& forward, glm::vec3& up);

	status playBGM(const std::string& path, bool loop = true);
	void stopBGM();

	Sound* registerSound(const std::string& pResource);
	void playSound(Sound* pSound, const glm::vec3& pos);
private:
	GraphicsManager& mGraphicsManager;

	SoundQueue mSoundQueue;
	ALCdevice *dev;
	ALCcontext *ctx;
	struct stat statbuf;
	ALenum error;
	float directionvect[6];

	glm::vec3 mWorldDimensions;

	std::vector<Sound*> mSounds;
};


#endif // !AUDIO_MANAGER_H
