#ifndef _FFMPEG_RESAMPLE_H__
#define _FFMPEG_RESAMPLE_H__

#include "Object.h"

namespace ffmpeg {

struct Resample : Object
{
	SwrContext* context;
	int64_t inputChannelLayout, outputChannelLayout;
	AVSampleFormat inputSampleFormat, outputSampleFormat;
	int inputSampleRate, outputSampleRate;

	Resample()
		: 
		context(nullptr), 
		inputChannelLayout(0), inputSampleFormat(AVSampleFormat::AV_SAMPLE_FMT_NONE), inputSampleRate(0),
		outputChannelLayout(0), outputSampleFormat(AVSampleFormat::AV_SAMPLE_FMT_NONE), outputSampleRate(0)
	{
	}
	virtual ~Resample() { free(); }

	SwrContext* operator ->() { return context; }
	SwrContext* operator &() { return context; }

	bool reset(int64_t inputChannelLayout, AVSampleFormat inputSampleFormat, int inputSampleRate, int64_t outputChannelLayout, AVSampleFormat outputSampleFormat, int outputSampleRate)
	{
		free();
		this->inputChannelLayout = inputChannelLayout;
		this->inputSampleFormat = inputSampleFormat;
		this->inputSampleRate = inputSampleRate;
		this->outputChannelLayout = outputChannelLayout;
		this->outputSampleFormat = outputSampleFormat;
		this->outputSampleRate = outputSampleRate;
		
		if (context = swr_alloc_set_opts(nullptr, outputChannelLayout, outputSampleFormat, outputSampleRate, inputChannelLayout, inputSampleFormat, inputSampleRate, AV_LOG_LEVEL, nullptr))
			ok = swr_init(context) >= 0;

		return ok;
	}

	bool reset(int64_t inputChannelLayout, AVSampleFormat sampleFormat, int sampleRate, int64_t outputChannelLayout) { return reset(inputChannelLayout, sampleFormat, sampleRate, outputChannelLayout, sampleFormat, sampleRate); }
	bool reset(int64_t channelLayout, AVSampleFormat inputSampleFormat, int sampleRate, AVSampleFormat outputSampleFormat) { return reset(channelLayout, inputSampleFormat, sampleRate, channelLayout, outputSampleFormat, sampleRate); }
	bool reset(int64_t channelLayout, AVSampleFormat sampleFormat, int inputSampleRate, int outputSampleRate) { return reset(channelLayout, sampleFormat, inputSampleRate, channelLayout, sampleFormat, outputSampleRate); }

	bool resample(AVFrame* inputFrame, AVFrame* outputFrame)
	{
		return ok && swr_convert(context, outputFrame->data, outputFrame->nb_samples, (const uint8_t**)inputFrame->data, inputFrame->nb_samples) >= 0;
	}

	void free()
	{
		if(context) swr_free(&context);
		context = nullptr;
		inputChannelLayout = outputChannelLayout = 0;
		inputSampleFormat = outputSampleFormat = AVSampleFormat::AV_SAMPLE_FMT_NONE;
		inputSampleRate = outputSampleRate = 0;
		ok = false;
	}
};

} //namespace ffmpeg

#endif // _FFMPEG_RESAMPLE_H__