TEMPLATE = lib
TARGET = qmlffmpeg
QT += qml quick network multimedia
CONFIG += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
uri = qmlffmpeg

# Input
SOURCES += \
    qmlffmpeg_plugin.cpp

HEADERS += \
    qmlffmpeg_plugin.h \
    ffmpegplayer.h \
    Channel.h \
    Client.h \
    Codec.h \
    Converter.h \
    Decoder.h \
    Encoder.h \
    Frame.h \
    Object.h \
    Resample.h \
    Scale.h

DISTFILES = qmldir \
    qmlffmpeg.pro.user

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

INCLUDEPATH += ./ffmpeg
win32:INCLUDEPATH += c:/mingw64_4.5.2_multilib/MSYS/local/x86_64-w64-mingw32/include
win32:CONFIG( debug, debug|release ):LIBS += -Lc:/mingw64_4.5.2_multilib/MSYS/local/x86_64-w64-mingw32/lib

