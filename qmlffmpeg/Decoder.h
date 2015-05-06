#ifndef _FFMPEG_DECODER_H__
#define _FFMPEG_DECODER_H__

#include "Codec.h"

namespace ffmpeg {

struct Decoder : Codec
{
	int size;

	Decoder() {}
	virtual ~Decoder(){}

	virtual AVCodec* find(AVCodecID id) { return avcodec_find_decoder(id); }
	virtual AVCodec* find(const char* name) { return avcodec_find_decoder_by_name(name); }

	bool copy(AVCodecContext* context)
	{
		if(this->context)
			if(!avcodec_copy_context(this->context, context)) //NB! memory leak
				return true;
		return false;
	}

	bool reset(AVCodecContext* context) { return Codec::reset(context->codec_id) && copy(context); }

	bool decode(AVPacket* packet, AVFrame* output) 
	{ 
		if(packet->size > 0) 
		{
			int complete = 0;
			int size = -1;

			if(AVMEDIA_TYPE_VIDEO == context->codec_type)
				size = avcodec_decode_video2(context, output, &complete, packet);
			else if(AVMEDIA_TYPE_AUDIO == context->codec_type)
				size = avcodec_decode_audio4(context, output, &complete, packet);

			if(size >= 0)
			{
				packet->size -= size;
				packet->data += size;
				return complete;
			}
			else packet->size = 0;
		}
		return false;
	}
};

} //namespace ffmpeg

#endif // _FFMPEG_DECODER_H__