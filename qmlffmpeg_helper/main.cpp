#include <QApplication>
#include <QQmlApplicationEngine>
#include <qqml.h>

#include "ffmpegplayer.h"

FFMPEGLogger* FFMPEGLogger::_instance = nullptr;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<FFMPEGPlayer>("qmlffmpeg", 1, 0, "FFMPEGPlayer");
    qmlRegisterSingletonType<FFMPEGLogger>("qmlffmpeg", 1, 0, "FFMPEGLogger", FFMPEGLogger::provider);
    qRegisterMetaType<FFMPEGLogger::Level>("Level");
    qRegisterMetaType<FFMPEGParams>("FFMPEGParams");
    engine.load(QUrl(QStringLiteral("qrc:/qmlffmpeg.qml")));

    return app.exec();
}
