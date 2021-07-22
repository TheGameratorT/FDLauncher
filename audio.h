#pragma once

/*
 * Because Qt Multimedia sucks.
 */

#include <QVector>
#include <stdint.h>

#include <AL/al.h>
#include <AL/alc.h>

class AudioDevice
{
public:
	bool openDevice(const char* deviceName = nullptr);
	bool closeDevice();

private:
	ALCdevice* _M_ALDevice;
	ALCcontext* _M_ALContext;
};

class AudioStream
{
public:
	bool loadOgg(const char* data, std::size_t dataSize);
	void play();
	void stop();
	void setVolume(float value);
	float getVolume();
	void setLooping(bool flag);
	ALint getState();
	void destroy();

private:
	QVector<char> _M_data;
	std::uint8_t _M_channels;
	std::int32_t _M_sampleRate;
	std::uint8_t _M_bitsPerSample;
	ALuint _M_buffer;
	ALenum _M_format;
	ALuint _M_source;
};

class AudioFader
{
public:
	AudioFader();
	~AudioFader();

	enum Type
	{
		FadeIn = 0,
		FadeOut
	};

	typedef void (*FadeDoneCallback)(AudioStream* stream);

	void fadeIn(AudioStream* stream, float fadeVolume, float fadeSpeed, FadeDoneCallback fadeDoneCallback = nullptr);
	void fadeOut(AudioStream* stream, float fadeVolume, float fadeSpeed, FadeDoneCallback fadeDoneCallback = nullptr);
	void update();
	void clear();

private:
	struct FadeTarget
	{
		AudioStream* stream;
		int fadeType = 0;
		float fadeVolume;
		float fadeSpeed;
		FadeDoneCallback fadeDoneCallback;
	};

	void fade(AudioStream* stream, int fadeType, float fadeVolume, float fadeSpeed, FadeDoneCallback fadeDoneCallback);
	void updateCommon(FadeTarget& target, int& i, float newVolume, bool done);

	QVector<FadeTarget> _M_targets;
};
