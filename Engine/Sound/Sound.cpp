#include "../../sources/Vobris/include/vorbis/vorbisfile.h"
#include "../Tools/Log.hpp"
#include "Sound.hpp"

#define OGG_ENDIAN 0

using namespace std;
/*
 * Struct that holds the RIFF data of the Wave file.
  * The RIFF data is the meta data information that holds,
   * the ID, size and format of the wave file
    */
struct RIFF_Header {
	char chunkID[4];
	long chunkSize;//size not including chunkSize or chunkID
	char format[4];
};

/*
 * Struct to hold fmt subchunk data for WAVE files.
  */
struct WAVE_Format {
	char subChunkID[4];
	long subChunkSize;
	short audioFormat;
	short numChannels;
	long sampleRate;
	long byteRate;
	short blockAlign;
	short bitsPerSample;
};

/*
* Struct to hold the data of the wave file
*/
struct WAVE_Data {
	char subChunkID[4]; //should contain the word data
	long subChunk2Size; //Stores the size of the data block
};

// Load a vorbis file into an OpenAL buffer
bool LoadVorbisBuffer(const char *name, ALuint buffer)
{
	bool success = false;
	FILE *fh;
	vorbis_info *info;
	OggVorbis_File oggFile;

	if (alIsBuffer(buffer) == AL_FALSE)
	{
		Log::error("LoadVorbisBuffer() - called with bad AL buffer!");
		return false;
	}

	fh = fopen(name, "rb");

	if (fh)
	{
		if (ov_open(fh, &oggFile, NULL, 0) == 0)
		{
			info = ov_info(&oggFile, -1);

			ogg_int64_t len = ov_pcm_total(&oggFile, -1) * info->channels * 2; // always 16 bit data

			uint8_t *data = (uint8_t *)malloc(len);

			if (data)
			{
				int bs = -1;
				ogg_int64_t todo = len;
				uint8_t *bufpt = data;

				while (todo)
				{
					int read = ov_read(&oggFile, (char *)bufpt, todo, OGG_ENDIAN, 2, 1, &bs);
					todo -= read;
					bufpt += read;
				}

				alBufferData(buffer, (info->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, data, len, info->rate);
				success = true;

				free(data);
			}
			else
				Log::error("LoadVorbisBuffer() - couldn't allocate decode buffer");

			ov_clear(&oggFile);
		}
		else
		{
			fclose(fh);
			Log::error("LoadVorbisBuffer() - ov_open_callbacks() failed, file isn't vorbis?");
		}
	}
	else
		Log::error("LoadVorbisBuffer() - couldn't open file!");


	return success;
}


bool loadWavFile(const std::string filename, ALuint* buffer,
	ALsizei* size, ALsizei* frequency, ALenum* format)
{
	//Local Declarations
	FILE* soundFile = NULL;
	WAVE_Format wave_format;
	RIFF_Header riff_header;
	WAVE_Data wave_data;
	unsigned char* data;

	try {
		soundFile = fopen(filename.c_str(), "rb");
		if (!soundFile)
			throw (filename);

		// Read in the first chunk into the struct
		fread(&riff_header, sizeof(RIFF_Header), 1, soundFile);

		//check for RIFF and WAVE tag in memeory
		if ((riff_header.chunkID[0] != 'R' ||
			riff_header.chunkID[1] != 'I' ||
			riff_header.chunkID[2] != 'F' ||
			riff_header.chunkID[3] != 'F') ||
			(riff_header.format[0] != 'W' ||
				riff_header.format[1] != 'A' ||
				riff_header.format[2] != 'V' ||
				riff_header.format[3] != 'E'))
			throw ("Invalid RIFF or WAVE Header");

		//Read in the 2nd chunk for the wave info
		fread(&wave_format, sizeof(WAVE_Format), 1, soundFile);
		//check for fmt tag in memory
		if (wave_format.subChunkID[0] != 'f' ||
			wave_format.subChunkID[1] != 'm' ||
			wave_format.subChunkID[2] != 't' ||
			wave_format.subChunkID[3] != ' ')
			throw ("Invalid Wave Format");

		//check for extra parameters;
		if (wave_format.subChunkSize > 16)
			fseek(soundFile, sizeof(short), SEEK_CUR);

		//Read in the the last byte of data before the sound file
		fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);
		//check for data tag in memory
		if (wave_data.subChunkID[0] != 'd' ||
			wave_data.subChunkID[1] != 'a' ||
			wave_data.subChunkID[2] != 't' ||
			wave_data.subChunkID[3] != 'a')
			throw ("Invalid data header");

		//Allocate memory for data
		data = new unsigned char[wave_data.subChunk2Size];

		// Read in the sound data into the soundData variable
		if (!fread(data, wave_data.subChunk2Size, 1, soundFile))
			throw ("error loading WAVE data into struct!");

		//Now we set the variables that we passed in with the
		//data from the structs
		*size = wave_data.subChunk2Size;
		*frequency = wave_format.sampleRate;
		//The format is worked out by looking at the number of
		//channels and the bits per sample.
		if (wave_format.numChannels == 1) {
			if (wave_format.bitsPerSample == 8)
				*format = AL_FORMAT_MONO8;
			else if (wave_format.bitsPerSample == 16)
				*format = AL_FORMAT_MONO16;
		}
		else if (wave_format.numChannels == 2) {
			if (wave_format.bitsPerSample == 8)
				*format = AL_FORMAT_STEREO8;
			else if (wave_format.bitsPerSample == 16)
				*format = AL_FORMAT_STEREO16;
		}
		//create our openAL buffer and check for success
		alGenBuffers(1, buffer);
		//errorCheck();
		//now we put our data into the openAL buffer and
		//check for success
		alBufferData(*buffer, *format, (void*)data, *size, *frequency);
		//errorCheck();
		//clean up and return true if successful
		fclose(soundFile);
		return true;
	}
	catch (std::string error) {
		//our catch statement for if we throw a string
		Log::error("Error trying to load %s", filename);
		//clean up memory if wave loading fails
		if (soundFile != NULL) fclose(soundFile);
		//return false to indicate the failure to load wave
		return false;
	}
}

Sound::Sound(const std::string& pResource) : mPath(pResource), mBuffer(0), mSource(0), mData(NULL)
{
	Log::info("Sound constructor");
}

status Sound::load() 
{
	Log::info("Loading sound %s", mPath);

	ALenum error = AL_NO_ERROR;
	std::string fileFormat = mPath.substr(mPath.size() - 3, std::string::npos);

	// Generate Buffers 
	alGetError(); // clear error code 
	alGenBuffers(1, &mBuffer);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		Log::error("alGenBuffers :", error);
		goto ERROR;
	}

	// load file
	if (fileFormat == "wav")
	{
		if (!loadWavFile(mPath, &mBuffer, &mSize, &mFrequency, &mFormat))
		{
			Log::error("Error loading wave sound %s", mPath);
			goto ERROR;
		}
	}
	else if (fileFormat == "ogg")
	{
		if (!LoadVorbisBuffer(mPath.c_str(), mBuffer))
		{
			Log::error("Error loading ogg sound %s", mPath);
			goto ERROR;
		}
	}
	else
	{
		Log::error("Unsupported file format");
		goto ERROR;
	}

	// Generate Sources 
	alGenSources(1, &mSource);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		Log::error("alGenSources 1 : ", error);
		return 1;
	}
	// Attach buffer 0 to source
	alSourcei(mSource, AL_BUFFER, mBuffer);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		Log::error("alSourcei AL_BUFFER 0 : ", error);
	}

	return STATUS_OK;
ERROR:
	Log::error("Error while reading audio file.");

	return STATUS_KO;
}

status Sound::unload() 
{
	alDeleteBuffers(1, &mBuffer);
	alDeleteSources(1, &mSource);

	mBuffer = NULL;
	mSource = NULL;

	return STATUS_OK;
}