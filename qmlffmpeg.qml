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
        //source: "f:/umayc/misc/testff/output/TAN BIONICA - Ella.mp4"
        //source: "f:/!!!/Свадьба в малиновке. Чует мое сердце, что мы накануне грандиозного шухера.mp4"
        //source: "f:/!!!/Шаолинь.avi"
        //source: "rtsp://8.15.251.47:1935/rtplive/FairfaxCCTV233"
        source: "rtsp://хуй:1935/rtplive/FairfaxCCTV233"
        onPlayingChanged: print('PLAYING', playing)
    }

    VideoOutput
    {
        source: _ffmpeg
        anchors.fill: parent
    }

    Column
    {
        anchors.right: parent.right
        Button
        {
            text: "play"
            onClicked: _ffmpeg.play()
        }
        Button
        {
            text: "stop"
            onClicked: _ffmpeg.stop()
        }
    }

    Component.onCompleted:
    {
        _ffmpeg.play()

        FFMPEGLogger.level = FFMPEGLogger.Error
        FFMPEGLogger.log.connect(pprint)
    }

    function pprint() { print(Array.prototype.slice.call(arguments).map(JSON.stringify)) }

}
