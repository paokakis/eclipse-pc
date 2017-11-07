#include "AudioManager.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "..\..\Engine\tools\Log.hpp"


AudioManager::AudioManager(GraphicsManager& graphMan) : mGraphicsManager(graphMan), mSoundQueue()
{
	Log::info("AudioManager constructor");
}

status AudioManager::start(const glm::vec3& worldDimens)
{
	// Initialization 
	mWorldDimensions = worldDimens;

	dev = alcOpenDevice(NULL); // select the "preferred dev" 
	if (dev)
	{
		ctx = alcCreateContext(dev, NULL);
		alcMakeContextCurrent(ctx);
	}
	else
	{
		Log::error("Error opening the sound device");
		goto ERROR;
	}
	// Check for EAX 2.0 support 
	// g_bEAX = alIsExtensionPresent("EAX2.0");

	mSoundQueue.initialize();

	// place listener at camera   
	ALfloat listenerPos[] = { 0.5,0.5,0.0 }; 
	ALfloat listenerVel[] = { 0.0,0.0,0.0 }; 
	ALfloat listenerOri[] = { 0.0,0.0,-1.0, 0.0,1.0,0.0 };

	// Position ... 
	alListenerfv(AL_POSITION,listenerPos); if ((error = alGetError()) != AL_NO_ERROR) 
	{  
		Log::error("alListenerfv POSITION : ", error); 
		goto ERROR;
	} 

	// Velocity ... 
	alListenerfv(AL_VELOCITY,listenerVel); if ((error = alGetError()) != AL_NO_ERROR) 
	{  
		Log::error("alListenerfv VELOCITY : ", error); 
		goto ERROR;
	} 

	// Orientation ... 
	alListenerfv(AL_ORIENTATION,listenerOri); if ((error = alGetError()) != AL_NO_ERROR) 
	{  
		Log::error("alListenerfv ORIENTATION : ", error);  
		goto ERROR;
	}

	return STATUS_OK;

ERROR:
	Log::error("Error starting Audio manager");

	return STATUS_KO;
}

status AudioManager::setListener(glm::vec3& pos, glm::vec3& velocity, glm::vec3& forward, glm::vec3& up)
{
	// place listener at camera   
	ALfloat listenerPos[] = { 
		(((pos.x / mWorldDimensions.x) - 0.5f) * 2.f),
		(((pos.y / mWorldDimensions.y) - 0.5f) * 2.f),
		(((pos.z / mWorldDimensions.z) - 0.5f) * 2.f) };
	ALfloat listenerVel[] = { 
		(((velocity.x / mWorldDimensions.x) - 0.5f) * 2.f),
		(((velocity.y / mWorldDimensions.y) - 0.5f) * 2.f),
		(((velocity.z / mWorldDimensions.z) - 0.5f) * 2.f) };
	ALfloat listenerOri[] = { 
		forward.x, forward.y, forward.z,
		up.x, up.y, up.z };

	// Position ... 
	alListenerfv(AL_POSITION, listenerPos); if ((error = alGetError()) != AL_NO_ERROR)
	{
		Log::error("alListenerfv POSITION : ", error);
		goto ERROR;
	}

	// Velocity ... 
	alListenerfv(AL_VELOCITY, listenerVel); if ((error = alGetError()) != AL_NO_ERROR)
	{
		Log::error("alListenerfv VELOCITY : ", error);
		goto ERROR;
	}

	// Orientation ... 
	alListenerfv(AL_ORIENTATION, listenerOri); if ((error = alGetError()) != AL_NO_ERROR)
	{
		Log::error("alListenerfv ORIENTATION : ", error);
		goto ERROR;
	}

	return STATUS_OK;

ERROR:
	Log::error("Error starting Audio manager");

	return STATUS_KO;
}

status AudioManager::playBGM(const std::string& path, bool loop)
{
	Sound* sound = registerSound(path.c_str());

	if (sound == NULL) goto ERROR;

	if (loop) alSourcei(*sound->getSource(), AL_LOOPING, AL_TRUE);

	alSourcePlay(*sound->getSource());

	return STATUS_OK;

ERROR:
	Log::error("Unable to play BGM");

	return STATUS_KO;
}

void AudioManager::stopBGM()
{
	alSourceStop(*mSounds[0]->getSource());
}

Sound* AudioManager::registerSound(const std::string& pResource)
{
	// Search in cache first
	std::vector<Sound*>::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		if ((*iter)->getPath() == pResource) 
		{ 
			Log::info("Found sound %s in cache", pResource.c_str());
			return (*iter);
		}
	}

	mSounds.push_back(new Sound(pResource));
	mSounds.back()->load();

	return mSounds.back();
}

void AudioManager::stop()
{
	// Exit 
	ctx = alcGetCurrentContext();
	dev = alcGetContextsDevice(ctx);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	alcCloseDevice(dev);
}

void AudioManager::playSound(Sound* pSound, const glm::vec3& pos)
{
	alSource3f(*pSound->getSource(), AL_POSITION, 
		(((pos.x / mWorldDimensions.x) - 0.5f) * 2.f),
		(((pos.y / mWorldDimensions.y) - 0.5f) * 2.f),
		(((pos.z / mWorldDimensions.z) - 0.5f) * 2.f));
	
	alSourcePlay(*pSound->getSource());
}