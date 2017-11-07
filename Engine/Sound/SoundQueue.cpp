#include "../Tools/Log.hpp"
#include "SoundQueue.hpp"

#define DATA_CHUNK_SIZE (1024)

SoundQueue::SoundQueue() : mBuffer(), mSource()
{}

/*Write initialize(), beginning with SLDataSource and SLDataSink to
describe the input and output channel. Use a SLDataFormat_PCM data format
(instead of SLDataFormat_MIME), which includes sampling, encoding, and
endianness information. Sounds need to be mono (that is, only one sound channel
for both left and right speakers when available). The queue is created with the
Android-specific extension SLDataLocator_AndroidSimpleBufferQueue()*/
status SoundQueue::initialize()
{
	Log::info("Starting sound player.");

	return STATUS_OK;
}

void SoundQueue::finalize() 
{
	Log::info("Stopping SoundQueue.");

	mBuffer = 0;
	mSource = 0;
}

void SoundQueue::playSound(Sound* pSound) 
{
	mBuffer = (ALuint*)pSound->getBuffer();
	mSource = (ALuint*)pSound->getSource();

	alSourceQueueBuffers(*pSound->getSource(), 1, pSound->getBuffer());

	return;
}