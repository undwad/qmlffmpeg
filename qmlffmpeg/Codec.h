#ifndef _FFMPEG_CODEC_H__
#define _FFMPEG_CODEC_H__

#include "Object.h"

namespace ffmpeg {

struct Codec : Object
{
	AVCodec* codec;
	AVCodecContext* context;
	void* buffer;

	Codec() : codec(nullptr), context(nullptr) {}
	virtual ~Codec() { free(); }

	AVCodecContext* operator ->() { return context; }
	AVCodecContext* operator &() { return context; }

	virtual AVCodec* find(AVCodecID id) = 0;
	virtual AVCodec* find(const char* name) = 0;

	bool alloc()
	{
		if(codec)
			if(context = avcodec_alloc_context3(codec))
			{
				context->codec_id = codec->id;
				context->codec_type = codec->type;
			}
		return nullptr != context;
	}

	bool reset(AVCodecID id)
	{
		free();
		if(codec = find(id))
			return alloc();
		return false;
	}

	bool reset(const char* name)
	{
		free();
		if(codec = find(name))
			return alloc();
		return false;
	}

	virtual bool open()
	{
		if(context && codec)
			ok = !avcodec_open2(context, codec, nullptr);
		return ok;
	}

	void free()
	{
		if(context)
		{
			avcodec_close(context);
			av_free(context);
		}
		context = nullptr;
		codec = nullptr;
		ok = false;
	}
};

} //namespace ffmpeg

#endif // _FFMPEG_CODEC_H__