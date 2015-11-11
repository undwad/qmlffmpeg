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
        params:
        ({
             //analyzeduration: 5000,
             //probesize: 20000,
             //video_size: '640x480',
             //pixel_format: 'yuv420p',
        })
        onPlayingChanged: print('PLAYING', playing)
        onVolumeChanged: print('volume', volume)
        onParamsChanged: pprint('PARAMS', params)
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
            "rtsp://bezreg:123456@85.237.42.235:5555/PSIA/streaming/channels/202",
            "http://80.153.71.35/cgi-bin/faststream.jpg?stream=full&fps=0",
            "http://oviso.axiscam.net/axis-cgi/mjpg/video.cgi",
            "http://oviso.axiscam.net/axis-cgi/mjpg/video.cgi?jpg",
            "http://q:__root__@192.168.10.211/axis-cgi/mjpg/video.cgi?camera=1&showlength=1",
            "rtsp://q:__root__@192.168.10.211/axis-media/media.amp/audio=0&camera=1",
            "f:/!!!/Шаолинь.avi",
            "f:/umayc/misc/testff/output/TAN BIONICA - Ella.mp4",
            "f:/!!!/Свадьба в малиновке. Чует мое сердце, что мы накануне грандиозного шухера.mp4",
            "rtsp://8.15.251.47:1935/rtplive/FairfaxCCTV233",
            "rtsp://хуй:1935/rtplive/FairfaxCCTV233"
        ]
    }

    ComboBox
    {
        id: _transport
        anchors.left: _urls.right
        anchors.top: parent.top
        model: [ '', 'tcp', 'http', 'udp' ]
        onCurrentTextChanged: if(currentText) _ffmpeg.params.rtsp_transport = currentText; else delete _ffmpeg.params.rtsp_transport
    }

    Slider
    {
        id: _volume
        anchors.left: _transport.right
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
        FFMPEGLogger.level = FFMPEGLogger.Warning
        FFMPEGLogger.log.connect(function(level, message, classname, url)
        {
            print(new Date(), FFMPEGLogger.levelToString(level), message, classname, url)
        })
    }

    function pprint() { print(Array.prototype.slice.call(arguments).map(JSON.stringify)) }

}
