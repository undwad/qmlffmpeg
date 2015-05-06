import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtMultimedia 5.4
import qmlffmpeg 1.0

ApplicationWindow
{
    title: qsTr("Hello World")
    width: 800
    height: 500
    visible: true

    FFMPEGPlayer
    {
        id: _ffmpeg
        source: "f:/umayc/misc/testff/output/TAN BIONICA - Ella.mp4"
    }

    VideoOutput
    {
        source: _ffmpeg
        anchors.fill: parent
    }

    Component.onCompleted: _ffmpeg.play()
}
