#ifndef _FFMPEG_SCALE_H__
#define _FFMPEG_SCALE_H__

#include "Object.h"

namespace ffmpeg {

struct Scale : Object
{
	static const int DefaultBrightness = 0;
	static const int DefaultContrast = 1 << 16;
	static const int DefaultSaturation = 1 << 16;

	SwsContext* context;
	int brightness, contrast, saturation;
	PixelFormat inputPixelFormat, outputPixelFormat;
	int inputWidth, inputHeight, outputWidth, outputHeight;

	Scale() 
		: 
		context(nullptr), brightness(DefaultBrightness), contrast(DefaultContrast), saturation(DefaultSaturation), 
		inputPixelFormat(PixelFormat::PIX_FMT_NONE), inputWidth(0), inputHeight(0), 
		outputPixelFormat(PixelFormat::PIX_FMT_NONE), outputWidth(0), outputHeight(0)
	{
	}
	virtual ~Scale() { free(); }

	SwsContext* operator ->() { return context; }
	SwsContext* operator &() { return context; }

	bool reset(PixelFormat inputPixelFormat, int inputWidth, int inputHeight, PixelFormat outputPixelFormat, int outputWidth, int outputHeight)
	{
		free();
		this->inputPixelFormat = inputPixelFormat;
		this->outputPixelFormat = outputPixelFormat;
		this->inputWidth = inputWidth;
		this->inputHeight = inputHeight;
		this->outputWidth = outputWidth;
		this->outputHeight = outputHeight;
		if(context = sws_getContext(inputWidth, inputHeight, inputPixelFormat, outputWidth, outputHeight, outputPixelFormat, SWS_BICUBIC, nullptr, nullptr, nullptr))
			ok = true;
		return ok;
	}

	bool reset(PixelFormat inputPixelFormat, int width, int height, PixelFormat outputPixelFormat) { return reset(inputPixelFormat, width, height, outputPixelFormat, width, height); }
	bool reset(PixelFormat pixelFormat, int inputWidth, int inputHeight, int outputWidth, int outputHeight) { return reset(pixelFormat, inputWidth, inputHeight, pixelFormat, outputWidth, outputHeight); }

	void scale(AVFrame* inputFrame, AVFrame* outputFrame)
	{
		if(ok)
		{
			const int inv_table[4] = { 104597, 132201, 25675, 53279 };
			sws_setColorspaceDetails(context, inv_table, 0, inv_table, 0, brightness, contrast, saturation);
			sws_scale(context, inputFrame->data, inputFrame->linesize, 0, inputHeight, outputFrame->data, outputFrame->linesize);
		}
	}

	void setBrightness(int value) { if(value >= -(1 << 16) && value <= (1 << 16)) brightness = value; }
	void setContrast(int value) { if(value >= 1 && value <= (1 << 17)) contrast = value; }
	void setSaturation(int value) { if(value >= 0 && value <= (1 << 17)) saturation = value; }

	void free()
	{
		if(context) av_free(context);
		context = nullptr;
		inputPixelFormat = outputPixelFormat = PixelFormat::PIX_FMT_NONE;
		inputWidth = inputHeight = outputWidth = outputHeight = 0;
		ok = false;
	}
};

} //namespace ffmpeg

#endif // _FFMPEG_SCALE_H__