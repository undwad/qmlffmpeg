#ifndef _FFMPEG_FRAME_H__
#define _FFMPEG_FRAME_H__

#include "Object.h"

#pragma warning(disable:4996)

namespace ffmpeg {

struct Frame : Object
{
	AVFrame* frame;
	void* buffer;
	PixelFormat pixelFormat;
	int width, height, size, align;
	AVSampleFormat sampleFormat;
	int channelCount, sampleCount;

	Frame() 
		: 
		frame(nullptr), 
		buffer(nullptr), 
		pixelFormat(PixelFormat::PIX_FMT_NONE), 
		width(0), 
		height(0), 
		size(0), 
		align(0), 
		sampleFormat(AVSampleFormat::AV_SAMPLE_FMT_NONE), 
		channelCount(0) ,
		sampleCount(0)
	{
	}
	virtual ~Frame() { free(); }

	AVFrame* operator ->() { return frame; }
	AVFrame* operator &() { return frame; }

	bool reset()
	{
		free();
        if(frame = av_frame_alloc())
			ok = true;
		return ok;
	}

	bool reset(PixelFormat pixelFormat, int width, int height, void* buffer = nullptr, int align = 1)
	{
		free();
		this->pixelFormat = pixelFormat;
		this->width = width;
		this->height = height;
		this->align = align;
		size = avpicture_get_size(pixelFormat, width, height);
		if(!buffer)
			this->buffer = buffer = av_malloc(size);
		if(buffer)
            if(frame = av_frame_alloc())
				ok = av_image_fill_arrays(frame->data, frame->linesize, (uint8_t*)buffer, pixelFormat, width, height, align) >= 0;
		return ok;
	}

	bool reset(AVSampleFormat sampleFormat, int channelCount, int sampleCount, void* buffer = nullptr, int align = 1)
	{
		free();
		this->sampleFormat = sampleFormat;
		this->channelCount = channelCount;
		this->sampleCount = sampleCount;
		this->align = align;
		size = av_samples_get_buffer_size(nullptr, channelCount, sampleCount, sampleFormat, align);
		if(!buffer)
			this->buffer = buffer = av_malloc(size);
		if(buffer)
            if(frame = av_frame_alloc())
			{
				frame->format = sampleFormat;
				frame->nb_samples = sampleCount;
				ok = avcodec_fill_audio_frame(frame, channelCount, sampleFormat, (uint8_t*)buffer, size, align) >= 0;
			}
		return ok;
	}

	void defaults()	{ avcodec_get_frame_defaults(frame); }

	void flip()
	{
		if(ok)
		{
			frame->data[0] += frame->linesize[0] * (height - 1);
			frame->linesize[0] = -frame->linesize[0];
		}
	}

	bool myBuffer(const void* buffer) { return frame && buffer == frame->data[0]; }

	void free()
	{
        if(frame) av_frame_free(&frame);
		if(buffer) av_free(buffer);
		frame = nullptr;
		buffer = nullptr;
		pixelFormat = PixelFormat::PIX_FMT_NONE;
		sampleFormat = AVSampleFormat::AV_SAMPLE_FMT_NONE; 
		width = height = size = channelCount = align = 0;
		ok = false;
	}
};

} //namespace ffmpeg

#endif // _FFMPEG_FRAME_H__
