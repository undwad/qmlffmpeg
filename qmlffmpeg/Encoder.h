#ifndef _FFMPEG_ENCODER_H__
#define _FFMPEG_ENCODER_H__

#include "Codec.h"

namespace ffmpeg {

struct Encoder : Codec
{
	Encoder() {}
	virtual ~Encoder(){}

	virtual AVCodec* find(AVCodecID id) { return avcodec_find_encoder(id); }
	virtual AVCodec* find(const char* name) { return avcodec_find_encoder_by_name(name); }

	virtual bool open()
	{
		context->pix_fmt = codec->pix_fmts[0];
		return Codec::open();
	}

	bool encode(AVFrame* frame, AVPacket* packet) 
	{ 
		int got_packet = 0;
		return !avcodec_encode_video2(context, packet, frame, &got_packet) && got_packet;
	}
};

} //namespace ffmpeg

#endif // _FFMPEG_ENCODER_H__