#include <QApplication>
#include <QQmlApplicationEngine>
#include <qqml.h>

#include "ffmpegplayer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<FFMPEGPlayer>("qmlffmpeg", 1, 0, "FFMPEGPlayer");
    engine.load(QUrl(QStringLiteral("qrc:/qmlffmpeg.qml")));

    return app.exec();
}
