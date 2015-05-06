/*
** ffmpeg media stream player binding for qtmultimedia
** https://github.com/undwad/qmlffmpeg mailto:undwad@mail.ru
** see copyright notice in ./LICENCE
*/

#pragma once

#include <QObject>
#include <QString>
#include <QQuickItem>
#include <QList>
#include <QThread>
#include <QByteArray>
#include <QDebug>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QVideoFrame>
#include <QAudioFormat>
#include <QAudioOutput>

#include <climits>
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <ctime>
#include <chrono>
#include <queue>

using namespace std;
using namespace std::chrono;

#define AV_LOG_LEVEL AV_LOG_VERBOSE

using namespace std;

#include "Channel.h"

class FFMPEGWorker : public QObject, ffmpeg::Channel
{
    Q_OBJECT

public:
    FFMPEGWorker() : ffmpeg::Channel(AVPixelFormat::AV_PIX_FMT_BGRA, AV_SAMPLE_FMT_S16)
    {
    }

    virtual bool interrupt() { return !_playing; }

    virtual void* createImage(int width, int height, int& align)
    {
        QVideoSurfaceFormat format(QSize(width, height), QVideoFrame::Format_BGR32);
        format.setFrameRate(videoCodec->time_base.num / videoCodec->time_base.den);
        emit startVideoSurface(format);
        _image.resize(width * height * av_get_bits_per_pixel(av_pix_fmt_desc_get(pixelFormat)) / 8);
        return _image.data();
    }
    virtual void lockImage() { }
    virtual void unlockImage() { }

    virtual void* createSound(AVSampleFormat sampleFormat, int channelCount, int sampleCount, int bitRate, int sampleRate, int& align)
    {
        _sound.resize(av_samples_get_buffer_size(nullptr, channelCount, sampleCount, sampleFormat, align));
        QAudioFormat format;
        format.setChannelCount(channelCount);
        format.setCodec("audio/pcm");
        format.setSampleRate(sampleRate);
        format.setSampleSize(bitRate);
        switch(sampleFormat)
        {
        case AV_SAMPLE_FMT_U8:
        case AV_SAMPLE_FMT_U8P: format.setSampleType(QAudioFormat::UnSignedInt); break;
        case AV_SAMPLE_FMT_S16:
        case AV_SAMPLE_FMT_S32:
        case AV_SAMPLE_FMT_S16P:
        case AV_SAMPLE_FMT_S32P: format.setSampleType(QAudioFormat::SignedInt); break;
        case AV_SAMPLE_FMT_FLT:
        case AV_SAMPLE_FMT_DBL:
        case AV_SAMPLE_FMT_FLTP:
        case AV_SAMPLE_FMT_DBLP: format.setSampleType(QAudioFormat::Float); break;
        default: format.setSampleType(QAudioFormat::Unknown);
        }
        _audioOutput = new QAudioOutput(format);
        _audioInput = _audioOutput->start();
        tryEmitAudioOutputError();
        return _sound.data();
    }

    virtual void lockSound() { }
    virtual void unlockSound() { }

    virtual void free()
    {
        ffmpeg::Channel::free();
        if(_audioOutput) delete _audioOutput;
    }

public slots:
    void playing(const QString& source)
    {
        _playing = true;
        reset(source.toStdString().c_str());
        int ptimestamp = 0;
        while(_playing && receive())
        {
            if(hasAudio())
            {
                int timestamp = popAudioSample();
                if (timestamp >= 0)
                {
                    _audioInput->write(_sound.data(), _sound.size());
                        //audioTimeStamp = timestamp;
                        //result = sound;
                }
            }
            if(hasVideo())
            {
                int timestamp = popVideoFrame();
                if(timestamp >= 0)
                {
                    QVideoFrame frame(_image.size(), QSize(videoCodec->width, videoCodec->height), _image.size() / videoCodec->height, QVideoFrame::Format_BGR32);
                    frame.setStartTime(ptimestamp * 1000);
                    frame.setEndTime(timestamp * 1000);
                    if(frame.map(QAbstractVideoBuffer::WriteOnly))
                    {
                        memcpy(frame.bits(), _image.data(), _image.size());
                        frame.unmap();
                        emit presentVideoSurface(frame);
                    }
                    ptimestamp = timestamp;
                }
            }
        }
        emit stopVideoSurface();
    }

    void stopping() { _playing = false; }

signals:
    void startVideoSurface(const QVideoSurfaceFormat& format);
    void presentVideoSurface(const QVideoFrame& frame);
    void stopVideoSurface();
    void audioOutputError(QAudio::Error error);

private:
    QByteArray _image;
    QByteArray _sound;
    bool _playing = false;
    QAudioOutput* _audioOutput = nullptr;
    QIODevice* _audioInput = nullptr;

    void tryEmitAudioOutputError()
    {
        if(_audioOutput && QAudio::NoError != _audioOutput->error())
            emit audioOutputError(_audioOutput->error());
    }
};

class FFMPEGPlayer : public QObject
{
public:
    Q_OBJECT

    Q_PROPERTY(QString source MEMBER _source)
    Q_PROPERTY(QAbstractVideoSurface* videoSurface MEMBER _videoSurface)

    Q_DISABLE_COPY(FFMPEGPlayer)

signals:
    void playing(const QString& source);
    void stopping();
    void error(const QString& error);

public:
    FFMPEGPlayer(QQuickItem *parent = 0) : QObject(parent)
    {
        _worker->moveToThread(&_thread);
        connect(&_thread, &QThread::finished, _worker, &QObject::deleteLater);
        connect(this, &FFMPEGPlayer::playing, _worker, &FFMPEGWorker::playing);
        connect(this, &FFMPEGPlayer::stopping, _worker, &FFMPEGWorker::stopping);
        connect(_worker, &FFMPEGWorker::startVideoSurface, this, [this](const QVideoSurfaceFormat& format)
        {
            if(!_videoSurface->start(format))
                emitVideoSurfaceError();
        });
        connect(_worker, &FFMPEGWorker::presentVideoSurface, this, [this](const QVideoFrame& frame)
        {
            //qDebug() << frame.size() << frame.startTime() << frame.endTime();
            if(!_videoSurface->present(frame))
                emitVideoSurfaceError();
        });
        connect(_worker, &FFMPEGWorker::stopVideoSurface, this, [this]()
        {
            _videoSurface->stop();
        });
        connect(_worker, &FFMPEGWorker::audioOutputError, this, &FFMPEGPlayer::audioOutputError);
        _thread.start();
    }

    ~FFMPEGPlayer()
    {
        stop();
        _thread.quit();
        _thread.wait();
    }

public slots:
    void play() { emit playing(_source); }

    void stop() { emit stopping(); }

private slots:
    void audioOutputError(QAudio::Error error)
    {
        switch (error)
        {
        case QAudio::OpenError: emit this->error("error occurred opening the audio device"); break;
        case QAudio::IOError: emit this->error("error occurred during read/write of audio device"); break;
        case QAudio::UnderrunError: emit this->error("audio data is not being fed to the audio device at a fast enough rate"); break;
        case QAudio::FatalError: emit this->error("non-recoverable error has occurred, the audio device is not usable at this time"); break;
        }
    }

private:
    QString _source;
    QAbstractVideoSurface* _videoSurface;
    FFMPEGWorker* _worker = new FFMPEGWorker;
    QThread _thread;

    void emitVideoSurfaceError()
    {
        switch (_videoSurface->error())
        {
        case QAbstractVideoSurface::UnsupportedFormatError: emit error("video format was not supported"); break;
        case QAbstractVideoSurface::IncorrectFormatError: emit error("video frame was not compatible with the format of the surface"); break;
        case QAbstractVideoSurface::StoppedError: emit error("surface has not been started"); break;
        case QAbstractVideoSurface::ResourceError: emit error("surface could not allocate some resource"); break;
        }
    }
};
