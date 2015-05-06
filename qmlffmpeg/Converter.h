#ifndef _FFMPEG_CONVERTER_H__
#define _FFMPEG_CONVERTER_H__

#include "Client.h"
#include "Frame.h"
#include "Decoder.h"
#include "Encoder.h"

namespace ffmpeg {

struct Converter : Client
{
	AVRational timeBase;
	Frame frame;
	AVCodecID inputCodecID, outputCodecID;
	bool convert;
	Decoder decoder;
	Encoder encoder;

	Converter() : inputCodecID(AV_CODEC_ID_NONE), outputCodecID(AV_CODEC_ID_NONE) {}
	virtual ~Converter() { }

	virtual void onVideoStream(AVStream* stream)
	{
		timeBase = stream->time_base;
		inputCodecID = stream->codec->codec_id;
		outputCodecID = convertCodecID(inputCodecID);
		if(inputCodecID != outputCodecID)
		{
			if(decoder.reset(stream->codec))
				decoder.open();
			if(encoder.reset(outputCodecID))
			{
				encoder.context->pix_fmt = stream->codec->pix_fmt;
				encoder.context->width = stream->codec->width;
				encoder.context->height = stream->codec->height;
				encoder.context->time_base = stream->codec->time_base;
				encoder.open();
			}
			ok = decoder.ok && encoder.ok;
		}
	}

	virtual void onVideoPacket(AVPacket* packet)
	{
		if(inputCodecID == outputCodecID)
			onVideoPacket(packet->data, packet->size, packet->pts * 1000 * timeBase.num / timeBase.den);
		else
		{
			if(!frame || frame.width != decoder->width || frame.height != decoder->height || frame.pixelFormat != decoder->pix_fmt)
				frame.reset(decoder->pix_fmt, decoder->width, decoder->height);

			for(auto p = *packet; p.size > 0;) 
				if(decoder.decode(&p, &frame)) 
				{
					AVPacket output;
					output.data = nullptr;
					output.size = 0;
					if(encoder.encode(&frame, &output))
					{
						onVideoPacket(output.data, output.size, p.pts * 1000 * timeBase.num / timeBase.den);
						//av_free_packet(&output);
					}
				}
		}
	}

	virtual AVCodecID convertCodecID(AVCodecID codecid) = 0;
	virtual void onVideoPacket(void* buffer, int size, int timestamp) = 0;

	virtual void free() 
	{ 
		Client::free(); 
		outputCodecID = inputCodecID = AV_CODEC_ID_NONE;
	}
};

} //namespace ffmpeg

#endif // _FFMPEG_CONVERTER_H__