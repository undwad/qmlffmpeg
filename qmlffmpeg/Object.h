#ifndef _FFMPEG_OBJECT_H__
#define _FFMPEG_OBJECT_H__

#ifndef AV_LOG_LEVEL
#	define AV_LOG_LEVEL AV_LOG_FATAL
#endif

extern "C"
{
#	include <libavutil/avutil.h>
#	include <libavcodec/avcodec.h>
#	include <libswscale/swscale.h>
#	include <libavformat/avformat.h>
#	include <libavformat/url.h>
#	include <libswresample/swresample.h>
#	include <libavutil/imgutils.h>
#	include <libavutil/log.h>
#	include <libavutil/channel_layout.h>
#	include <libavutil/samplefmt.h>
}

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libgcc.a")
#pragma comment(lib, "libmingwex.a")
#pragma comment(lib, "libavcodec.a")
#pragma comment(lib, "libavutil.a")
#pragma comment(lib, "libswscale.a")
#pragma comment(lib, "libswresample.a")
#pragma comment(lib, "libavformat.a")
//#pragma comment(lib, "libpthreadGC2.a")
//#pragma comment(lib, "iconv.lib")

namespace ffmpeg {

struct Object
{
	bool ok;

	static void init()
	{
		static bool initialized = false;
		if(!initialized)
		{	
#			ifdef CS
				av_lockmgr_register(av_cs_callback);
#			endif
			av_log_set_level(AV_LOG_LEVEL);
			av_register_all();
			avcodec_register_all();
			avformat_network_init();
			initialized = true;
		}
	}

	Object() : ok(false) { init(); }

	virtual ~Object() {}

	operator bool() { return ok; }
	bool operator !() { return !ok; }

#	ifdef CS
		static int av_cs_callback(void **cs, enum AVLockOp op)
		{
			switch(op)
			{
				case AV_LOCK_CREATE: *(CS**)cs = new CS; break;
				case AV_LOCK_OBTAIN: (*(CS**)cs)->lock(); break;
				case AV_LOCK_RELEASE: (*(CS**)cs)->unlock(); break;
				case AV_LOCK_DESTROY: delete *(CS**)cs; break;
			}
			return 0;
		}
#	endif
};

} //namespace ffmpeg

#endif // _FFMPEG_OBJECT_H__
