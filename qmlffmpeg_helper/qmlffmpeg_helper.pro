TEMPLATE = app

CONFIG += c++11

QT += qml quick widgets network multimedia

INCLUDEPATH += ./
INCLUDEPATH += ../qmlffmpeg/

SOURCES += main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    ../qmlwebsockets/websocketclient.h \
    ../qmlwebsockets/gunzip.h \
    ../qmlffmpeg/Channel.h \
    ../qmlffmpeg/Client.h \
    ../qmlffmpeg/Codec.h \
    ../qmlffmpeg/Converter.h \
    ../qmlffmpeg/Decoder.h \
    ../qmlffmpeg/Encoder.h \
    ../qmlffmpeg/ffmpegplayer.h \
    ../qmlffmpeg/Frame.h \
    ../qmlffmpeg/Object.h \
    ../qmlffmpeg/Resample.h \
    ../qmlffmpeg/Scale.h

INCLUDEPATH += ./ffmpeg
win32:INCLUDEPATH += c:/mingw64_4.5.2_multilib/MSYS/local/x86_64-w64-mingw32/include
win32:CONFIG( debug, debug|release ):LIBS += -Lc:/mingw64_4.5.2_multilib/MSYS/local/x86_64-w64-mingw32/lib    
