#ifndef HTTPERMANAGE_H
#define HTTPERMANAGE_H

#include <QObject>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>

using namespace std;

class httpermanage : QObject
{
    Q_OBJECT

public:
    httpermanage();
    void get_token(string url, string api, string key, string opt);
    void get_msg(QString url, QByteArray json, string opt);

    //百度云token
    QString audio_token;
    QString video_token;

private slots:
    //接收token值
    void recv_audio_token(QNetworkReply *reply);
    void recv_video_token(QNetworkReply *reply);

    //接收语音识别返回内容
    void recv_audio_msg(QNetworkReply *reply);
    //接收人脸检测返回内容
    void recv_video_msg(QNetworkReply *reply);

private:
    //百度云返回信息
    QString audio_msg;
    double video_msg;

    int video_flag;
};

#endif // HTTPERMANAGE_H
