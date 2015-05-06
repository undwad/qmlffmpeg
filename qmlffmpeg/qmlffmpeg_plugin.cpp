#include "qmlffmpeg_plugin.h"
#include "ffmpegplayer.h"

#include <qqml.h>

void QmlwebsocketsPlugin::registerTypes(const char *uri)
{
    // @uri qmlffmpeg
    qmlRegisterType<FFMPEGPlayer>(uri, 1, 0, "FFMPEGPlayer");
}


