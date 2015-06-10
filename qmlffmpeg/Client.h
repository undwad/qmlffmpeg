#ifndef _FFMPEG_CLIENT_H__
#define _FFMPEG_CLIENT_H__

#include "Object.h"

namespace ffmpeg {

struct Client : Object
{
	AVDictionary* dict;
	AVFormatContext* formatContext;
    int videoStreamIndex, audioStreamIndex;
	queue<AVPacket> videoQueue, audioQueue;

    Client() : dict(nullptr), formatContext(nullptr), videoStreamIndex(-1), audioStreamIndex(-1) {}
	virtual ~Client() { free(); }

    virtual bool reset(const char* url, const map<string, string>& params)
	{
		free();
		if(formatContext = avformat_alloc_context())
		{
			static const AVIOInterruptCB int_cb = {interrupt_cb, this};
			formatContext->interrupt_callback = int_cb;

            for(auto& param : params)
                av_dict_set(&dict, param.first.c_str(), param.second.c_str(), 0);

			if(0 == avformat_open_input(&formatContext, url, nullptr, &dict))
                if(0 <= avformat_find_stream_info(formatContext, nullptr))
				{
					for(unsigned int i = 0; i < formatContext->nb_streams; i++)
					{
						if(AVMEDIA_TYPE_VIDEO == formatContext->streams[i]->codec->codec_type)
							videoStreamIndex = i; 
						else if(AVMEDIA_TYPE_AUDIO == formatContext->streams[i]->codec->codec_type)
							audioStreamIndex = i;
					}

					if(ok = (videoStreamIndex >= 0 || audioStreamIndex >= 0))
					{
						if(videoStreamIndex >= 0)
							onVideoStream(formatContext->streams[videoStreamIndex]);
						if(audioStreamIndex >= 0)
							onAudioStream(formatContext->streams[audioStreamIndex]);
						av_read_play(formatContext);
						return true;
					}
				}
		}
		return false;
	}

	virtual void lockVideoQueue() {}
	virtual void unlockVideoQueue() {}
	virtual void lockAudioQueue() {}
	virtual void unlockAudioQueue() {}

	virtual bool receive()
	{
		if(ok)
		{
			AVPacket packet;
            if(ok = !av_read_frame(formatContext, &packet))
			{
				if (packet.stream_index == videoStreamIndex)
				{
					lockVideoQueue();
					videoQueue.push(packet);
					unlockVideoQueue();
				}
				else if (packet.stream_index == audioStreamIndex)
				{
					lockAudioQueue();
					audioQueue.push(packet);
					unlockAudioQueue();
				}
				return true;
			}
			av_read_pause(formatContext);
		}
		return false;
	}

	AVPacket* peekVideoPacket()	
	{ 
		AVPacket* result = nullptr;
		lockVideoQueue();
		if (!videoQueue.empty())
			result = &videoQueue.front();
		unlockVideoQueue();
		return result; 
	}

	AVPacket* peekAudioPacket()
	{
		AVPacket* result = nullptr;
		lockAudioQueue();
		if (!audioQueue.empty())
			result = &audioQueue.front();
		unlockAudioQueue();
		return result;
	}

	bool popVideoPacket()
	{
		bool result = false;
		lockVideoQueue();
		if (result = !videoQueue.empty())
		{
			av_free_packet(&videoQueue.front());
			videoQueue.pop();
		}
		unlockVideoQueue();
		return result;
	}

	bool popAudioPacket()
	{
		bool result = false;
		lockAudioQueue();
		if (result = !audioQueue.empty())
		{
			av_free_packet(&audioQueue.front());
			audioQueue.pop();
		}
		unlockAudioQueue();
		return result;
	}

	virtual void onVideoStream(AVStream* stream) = 0;
	virtual void onAudioStream(AVStream* stream) = 0;
    virtual bool interrupt() { return false; }

	virtual bool hasVideo() { return videoStreamIndex >= 0; }
	virtual bool hasAudio() { return audioStreamIndex >= 0; }

	bool IsVideoQueueEmpty() 
	{
		bool result = false;
		lockVideoQueue();
		result = videoQueue.empty();
		unlockVideoQueue();
		return result;
	}

	bool IsAudioQueueEmpty()
	{
		bool result = false;
		lockAudioQueue();
		result = audioQueue.empty();
		unlockAudioQueue();
		return result;
	}

	virtual void free()
	{
		if(dict) av_dict_free(&dict);
		if(formatContext) 
		{
			avformat_close_input(&formatContext);
			avformat_free_context(formatContext);
		}
		dict = nullptr;
		formatContext = nullptr;
		videoStreamIndex = audioStreamIndex = -1;
		while (popVideoPacket());
		while (popAudioPacket());
		ok = false;
	}

    static int interrupt_cb(void *p) { return reinterpret_cast<ffmpeg::Client*>(p)->interrupt() ? 1 : 0; }
};

} //namespace ffmpeg

#endif // _FFMPEG_CLIENT_H__
