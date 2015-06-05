#include "qmlffmpeg_plugin.h"
#include "ffmpegplayer.h"

#include <qqml.h>

FFMPEGLogger* FFMPEGLogger::_instance = nullptr;

void QmlwebsocketsPlugin::registerTypes(const char *uri)
{
    // @uri qmlffmpeg
    qmlRegisterType<FFMPEGPlayer>(uri, 1, 0, "FFMPEGPlayer");
    qmlRegisterSingletonType<FFMPEGLogger>(uri, 1, 0, "FFMPEGLogger", FFMPEGLogger::provider);
    qRegisterMetaType<FFMPEGLogger::Level>("Level");
    qRegisterMetaType<FFMPEGParams>("FFMPEGParams");
}


