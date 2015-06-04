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
        volume: _volume.value
        source: _urls.currentText
        onPlayingChanged: print('PLAYING', playing)
        onVolumeChanged: print('volume', volume)
    }

    VideoOutput
    {
        source: _ffmpeg
        anchors.fill: parent
    }

    ComboBox
    {
        id: _urls
        anchors.left: parent.left
        anchors.top: parent.top
        model:
        [
            "http://q:__root__@192.168.10.211/axis-cgi/mjpg/video.cgi?camera=1&showlength=1",
            "f:/!!!/Шаолинь.avi",
            "f:/umayc/misc/testff/output/TAN BIONICA - Ella.mp4",
            "f:/!!!/Свадьба в малиновке. Чует мое сердце, что мы накануне грандиозного шухера.mp4",
            "rtsp://8.15.251.47:1935/rtplive/FairfaxCCTV233",
            "rtsp://хуй:1935/rtplive/FairfaxCCTV233"
        ]

    }

    Slider
    {
        id: _volume
        anchors.left: _urls.right
        anchors.right: _buttons.left
        anchors.top: parent.top
        enabled: _ffmpeg.volume >= 0
        minimumValue: 0
        stepSize: 0.1
        maximumValue: 1
        value: 1
    }

    Column
    {
        id: _buttons
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
        FFMPEGLogger.level = FFMPEGLogger.Error
        FFMPEGLogger.log.connect(pprint)
    }

    function pprint() { print(Array.prototype.slice.call(arguments).map(JSON.stringify)) }

}
