#ifndef _FFMPEG_CHANNEL_H__
#define _FFMPEG_CHANNEL_H__

#include "Client.h"
#include "Frame.h"
#include "Decoder.h"
#include "Scale.h"
#include "Resample.h"

namespace ffmpeg {

struct Channel : Client
{
	AVRational timeBase;
	int desiredWidth, desiredHeight, desiredChannelCount, desiredSampleCount, desiredSampleRate;
	Frame inputVideo, outputVideo, inputAudio, outputAudio;
	Decoder videoCodec, audioCodec;
	Scale scale;
	Resample resample;
	AVPixelFormat pixelFormat;
	AVSampleFormat sampleFormat;
	AVPacket videoPacket, audioPacket;
	bool restart;

	Channel(AVPixelFormat pixfmt, AVSampleFormat smplfmt)
		: 
		pixelFormat(pixfmt), 
		sampleFormat(smplfmt),
		desiredWidth(0),
		desiredHeight(0),
		desiredChannelCount(0),
		desiredSampleCount(0),
		desiredSampleRate(0),
		videoPacket({ 0 }),
		audioPacket({ 0 }),
		restart(false) 
	{ }

	virtual ~Channel() { }

	virtual void onVideoStream(AVStream* stream)
	{
		timeBase = stream->time_base;
		if (videoCodec.reset(stream->codec) && videoCodec.open())
		{
			inputVideo.reset(videoCodec->pix_fmt, videoCodec->width, videoCodec->height);			
			int width = desiredWidth > 0 && desiredWidth < videoCodec->width ? desiredWidth : videoCodec->width;
			int height = desiredHeight > 0 && desiredHeight < videoCodec->height ? desiredHeight : videoCodec->height;
			int align = 1;
			if (auto image = createImage(width, height, align))
				if (scale.reset(videoCodec->pix_fmt, videoCodec->width, videoCodec->height, pixelFormat, width, height))
					outputVideo.reset(pixelFormat, width, height, image, align);
		}
	}

	int popVideoFrame(bool* keyframe = nullptr)
	{
		int result = -1;

		while (result < 0)
		{
			if (videoPacket.size <= 0)
			{
				if (auto packet = peekVideoPacket())
					videoPacket = *packet;
				else
					return -1;
			}

			while (videoPacket.size > 0)
				if (videoCodec.decode(&videoPacket, &inputVideo))
				{
					if (keyframe)
						*keyframe = inputVideo.frame->key_frame;

					lockImage();
					if (!restart && outputVideo)
					{
						scale.scale(&inputVideo, &outputVideo);
						result = videoPacket.pts * 1000 * timeBase.num / timeBase.den;
					}
					unlockImage();

					break;
				}

			if (videoPacket.size <= 0)
				popVideoPacket();
		}

		return result;
	}

	virtual void onAudioStream(AVStream* stream)
	{
		if (audioCodec.reset(stream->codec) && audioCodec.open())
		{
			inputAudio.reset(audioCodec->sample_fmt, audioCodec->channels, audioCodec->frame_size);
			int channelCount = this->desiredChannelCount ? this->desiredChannelCount : audioCodec->channels;
			int sampleCount = this->desiredSampleCount ? this->desiredSampleCount : audioCodec->frame_size;
			int sampleRate = this->desiredSampleRate ? this->desiredSampleRate : audioCodec->sample_rate;
			int align = 1;
			if (auto sound = createSound(sampleFormat, channelCount, sampleCount, audioCodec->bits_per_coded_sample, sampleRate, align))
				if (resample.reset(audioCodec->channel_layout, audioCodec->sample_fmt, audioCodec->sample_rate, av_get_default_channel_layout(channelCount), sampleFormat, sampleRate))
					outputAudio.reset(sampleFormat, channelCount, sampleCount, sound, align);
		}
	}

	int popAudioSample()
	{
		int result = -1;

		while (result < 0)
		{
			if (audioPacket.size <= 0)
			{
				if (auto packet = peekAudioPacket())
					audioPacket = *packet;
				else
					return -1;
			}

			while (audioPacket.size > 0)
				if (audioCodec.decode(&audioPacket, &inputAudio))
				{
					if (!restart && outputAudio)
						if (resample.resample(&inputAudio, &outputAudio))
							result = audioPacket.pts * 1000 * 1 / audioCodec->sample_rate;

					break;
				}

			if (audioPacket.size <= 0)
				popAudioPacket();
		}

		return result;
	}

	virtual void lockImage() = 0;
	virtual void* createImage(int width, int height, int& align) = 0;
	virtual void unlockImage() = 0;
	
	virtual void* createSound(AVSampleFormat sampleFormat, int channelCount, int sampleCount, int bitRate, int sampleRate, int& align) = 0;
	
	virtual bool hasVideo() { return Client::hasVideo() && outputVideo; }
	virtual bool hasAudio() { return Client::hasAudio() && outputAudio; }

	virtual void free() 
	{ 
		inputVideo.free();
		outputVideo.free();
		inputAudio.free();
		outputAudio.free();
		scale.free();
		resample.free();
		videoCodec.free();
		audioCodec.free();
		Client::free();
	}
};

} //namespace ffmpeg

#endif // _FFMPEG_CHANNEL_H__