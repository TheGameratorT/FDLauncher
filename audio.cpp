#include "audio.h"

/*
 * Because Qt Multimedia sucks.
 */

#include <iostream>

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

struct OggDataReader
{
	const char* data;
	std::size_t dataSize;
	std::size_t cursor;
};

static std::size_t read_ogg_callback(void* ptr, std::size_t size, std::size_t nmemb, void* datasource)
{
	OggDataReader* reader = static_cast<OggDataReader*>(datasource);

	std::size_t length = size * nmemb;
	if(reader->cursor + length > reader->dataSize)
		length = reader->dataSize - reader->cursor;

	std::memcpy(ptr, &reader->data[reader->cursor], length);
	reader->cursor += length;

	return length;
}

static int seek_ogg_callback(void* datasource, ogg_int64_t offset, int whence)
{
	OggDataReader* reader = static_cast<OggDataReader*>(datasource);
	if (whence == SEEK_SET) {
		reader->cursor = offset;
		return 0;
	}
	if (whence == SEEK_CUR) {
		reader->cursor += offset;
		return 0;
	}
	if (whence == SEEK_END) {
		reader->cursor = reader->dataSize - offset;
		return 0;
	}
	return -1;
}

static long tell_ogg_callback(void* datasource)
{
	return static_cast<OggDataReader*>(datasource)->cursor;
}

bool AudioDevice::openDevice(const char* deviceName)
{
	_M_ALDevice = alcOpenDevice(deviceName);
	if (!_M_ALDevice)
	{
		std::cerr << "ERROR: Could not open audio device" << std::endl;
		return false;
	}

	_M_ALContext = alcCreateContext(_M_ALDevice, nullptr);
	if (!_M_ALContext)
	{
		std::cerr << "ERROR: Could not create audio context" << std::endl;
		return false;
	}

	if (!alcMakeContextCurrent(_M_ALContext))
	{
		std::cerr << "ERROR: Could not make audio context current" << std::endl;
		return false;
	}

	return true;
}

bool AudioDevice::closeDevice()
{
	if (!alcMakeContextCurrent(nullptr))
	{
		std::cerr << "ERROR: Could not clear current audio context" << std::endl;
		return false;
	}

	alcDestroyContext(_M_ALContext);

	if (!alcCloseDevice(_M_ALDevice))
	{
		std::cerr << "ERROR: Could not close audio device" << std::endl;
		return false;
	}
	
	return true;
}

bool AudioStream::loadOgg(const char* data, std::size_t dataSize)
{
	OggVorbis_File oggVorbisFile;

	ov_callbacks oggCallbacks;
	oggCallbacks.read_func = read_ogg_callback;
	oggCallbacks.close_func = nullptr;
	oggCallbacks.seek_func = seek_ogg_callback;
	oggCallbacks.tell_func = tell_ogg_callback;

	OggDataReader reader;
	reader.data = data;
	reader.dataSize = dataSize;
	reader.cursor = 0;

	if (ov_open_callbacks(&reader, &oggVorbisFile, nullptr, -1, oggCallbacks) < 0)
	{
		std::cerr << "ERROR: Could not ov_open_callbacks " << std::endl;
		return false;
	}

	vorbis_info* vorbisInfo = ov_info(&oggVorbisFile, -1);

	_M_channels = vorbisInfo->channels;
	_M_bitsPerSample = 16;
	_M_sampleRate = vorbisInfo->rate;

	std::size_t decompSize = ov_pcm_total(&oggVorbisFile, -1);
	decompSize *= _M_channels * 2;

	_M_data.resize(decompSize);

	int bitstream = 0;
	std::size_t sizeRead = 0;
	while (sizeRead < decompSize)
	{
		long result = ov_read(&oggVorbisFile, &_M_data[sizeRead], 4096, 0, 2, 1, &bitstream);
		if (result == OV_HOLE) {
			std::cerr << "ERROR: OV_HOLE while reading ogg" << std::endl;
			return false;
		}
		else if (result == OV_EBADLINK) {
			std::cerr << "ERROR: OV_EBADLINK while reading ogg" << std::endl;
			return false;
		}
		else if (result == OV_EINVAL) {
			std::cerr << "ERROR: OV_EINVAL while reading ogg" << std::endl;
			return false;
		}
		else if (result == 0) {
			std::cerr << "ERROR: End of file reached while reading ogg" << std::endl;
			return false;
		}
		sizeRead += result;
	}

	alGenBuffers(1, &_M_buffer);

	if(_M_channels == 1 && _M_bitsPerSample == 8)
		_M_format = AL_FORMAT_MONO8;
	else if(_M_channels == 1 && _M_bitsPerSample == 16)
		_M_format = AL_FORMAT_MONO16;
	else if(_M_channels == 2 && _M_bitsPerSample == 8)
		_M_format = AL_FORMAT_STEREO8;
	else if(_M_channels == 2 && _M_bitsPerSample == 16)
		_M_format = AL_FORMAT_STEREO16;
	else
	{
		std::cerr
			<< "ERROR: unrecognised wave format: "
			<< _M_channels << " channels, "
			<< _M_bitsPerSample << " bps" << std::endl;
		return false;
	}

	alBufferData(_M_buffer, _M_format, _M_data.data(), _M_data.size(), _M_sampleRate);

	alGenSources(1, &_M_source);
	alSourcef(_M_source, AL_PITCH, 1);
	alSourcef(_M_source, AL_GAIN, 1.0f);
	alSource3f(_M_source, AL_POSITION, 0, 0, 0);
	alSource3f(_M_source, AL_VELOCITY, 0, 0, 0);
	alSourcei(_M_source, AL_LOOPING, AL_FALSE);
	alSourcei(_M_source, AL_BUFFER, _M_buffer);

	return true;
}

void AudioStream::play()
{
	alSourcePlay(_M_source);
}

void AudioStream::stop()
{
	alSourceStop(_M_source);
}

ALint AudioStream::getState()
{
	ALint state;
	alGetSourcei(_M_source, AL_SOURCE_STATE, &state);
	return state;
}

void AudioStream::setVolume(float value)
{
	alSourcef(_M_source, AL_GAIN, value);
}

float AudioStream::getVolume()
{
	ALfloat value;
	alGetSourcef(_M_source, AL_GAIN, &value);
	return value;
}

void AudioStream::setLooping(bool flag)
{
	alSourcei(_M_source, AL_LOOPING, flag);
}

void AudioStream::destroy()
{
	alDeleteSources(1, &_M_source);
	alDeleteBuffers(1, &_M_buffer);
}

AudioFader::AudioFader() {}
AudioFader::~AudioFader() {}

void AudioFader::fade(AudioStream* stream, int fadeType, float fadeVolume, float fadeSpeed, FadeDoneCallback fadeDoneCallback)
{
	FadeTarget target;
	target.stream = stream;
	target.fadeType = fadeType;
	target.fadeVolume = fadeVolume;
	target.fadeSpeed = fadeSpeed;
	target.fadeDoneCallback = fadeDoneCallback;
	_M_targets.push_back(target);
}

void AudioFader::fadeIn(AudioStream* stream, float fadeVolume, float fadeSpeed, FadeDoneCallback fadeDoneCallback)
{
	fade(stream, FadeIn, fadeVolume, fadeSpeed, fadeDoneCallback);
}

void AudioFader::fadeOut(AudioStream* stream, float fadeVolume, float fadeSpeed, FadeDoneCallback fadeDoneCallback)
{
	fade(stream, FadeOut, fadeVolume, fadeSpeed, fadeDoneCallback);
}

void AudioFader::updateCommon(FadeTarget& target, int& i, float newVolume, bool done)
{
	if (done) newVolume = target.fadeVolume;
	target.stream->setVolume(newVolume);
	if (done)
	{
		if (target.fadeDoneCallback != nullptr)
			target.fadeDoneCallback(target.stream);
		_M_targets.remove(i--);
	}
}

void AudioFader::update()
{
	for (int i = 0; i < _M_targets.size(); i++)
	{
		FadeTarget& target = _M_targets[i];
		switch(target.fadeType)
		{
		case FadeIn: {
			float newVolume = target.stream->getVolume();
			newVolume += target.fadeSpeed;
			updateCommon(target, i, newVolume, newVolume >= target.fadeVolume);
			break; }
		case FadeOut: {
			float newVolume = target.stream->getVolume();
			newVolume -= target.fadeSpeed;
			updateCommon(target, i, newVolume, newVolume <= target.fadeVolume);
			break; }
		}
	}
}

void AudioFader::clear()
{
	for (FadeTarget& target : _M_targets)
	{
		target.stream->stop();
		target.stream->destroy();
	}
}
