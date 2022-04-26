#include "httpermanage.h"

httpermanage::httpermanage()
{

}

void httpermanage::get_token(string url, string api, string key, string opt)
{
    QNetworkAccessManager *httper;
    QNetworkRequest req;
    QByteArray buf;

    httper = new QNetworkAccessManager;
    if (opt.compare("audio"))
    {
        connect(httper, SIGNAL(finished(QNetworkReply*)), this, SLOT(recv_audio_token(QNetworkReply*)));
        string str = url + "?grant_type=client_credentials&client_id=" + api + "&client_secret=" + key;
        req.setUrl(QUrl(QString(str.data())));
        httper->post(req, buf);
    }
    else if (opt.compare("video"))
    {
        connect(httper, SIGNAL(finished(QNetworkReply*)), this, SLOT(recv_video_token(QNetworkReply*)));
        string str = url + "?grant_type=client_credentials&client_id=" + api + "&client_secret=" + key;
        req.setUrl(QUrl(QString(str.data())));
        httper->post(req, buf);
    }
}

void httpermanage::get_msg(QString url, QByteArray json, string opt)
{
//    qDebug() << json.toStdString().data();
    qDebug() << url.toStdString().data();

    FILE *f = fopen("test.txt", "w+");
    char *str = json.data();
    fwrite(str, strlen(str), 1, f);
    fclose(f);

    QNetworkAccessManager *httper;
    httper = new QNetworkAccessManager;
    if (opt.compare("audio"))
    {
        connect(httper, SIGNAL(finished(QNetworkReply*)), this,
                SLOT(recv_audio_msg(QNetworkReply*)));

        QNetworkRequest request((QUrl(url)));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "Content-Type:application/json");
        httper->post(request, json);
    }
    else if (opt.compare("video"))
    {
        connect(httper, SIGNAL(finished(QNetworkReply*)), this,
                SLOT(recv_video_msg(QNetworkReply*)));

        QNetworkRequest request((QUrl(url)));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "Content-Type:application/json");
        httper->post(request, json);
    }
}

void httpermanage::recv_audio_token(QNetworkReply *reply)
{
    QByteArray buf = reply->readAll();

    QJsonDocument doc = QJsonDocument::fromJson(buf);
    //    qDebug() << doc;
    QJsonObject obj = doc.object();
    audio_token = obj.value("access_token").toString();

    qDebug() << "audio_token" << audio_token;
}

void httpermanage::recv_video_token(QNetworkReply *reply)
{
    QByteArray buf = reply->readAll();

    QJsonDocument doc = QJsonDocument::fromJson(buf);
    QJsonObject obj = doc.object();
    video_token = obj.value("access_token").toString();

    qDebug() << "video_token" << video_token;
}

void httpermanage::recv_audio_msg(QNetworkReply *reply)
{
    QByteArray buf = reply->readAll();
    QJsonDocument my_doucument = QJsonDocument::fromJson(buf); //接收回复的信息
    qDebug() << my_doucument; //打印回复的信息 - 调试使用
    if(my_doucument.isObject()) //解析回复的信息
    {
        QJsonObject obj = my_doucument.object();
        if(obj.contains("result"))
        {
            audio_msg = obj.value("result").toArray()[0].toString();
            qDebug() << audio_msg;
        }
    }
}

void httpermanage::recv_video_msg(QNetworkReply *reply)
{
    QByteArray buf = reply->readAll();
    QJsonDocument doucument = QJsonDocument::fromJson(buf); //接收回复的信息
    qDebug() << doucument; //打印回复的信息 - 调试使用
    if(doucument.isObject()) //解析回复的信息
    {
        QJsonObject obj = doucument.object();
        if(obj.contains("result"))
        {
            QJsonObject result = obj.value("result").toObject();
            if (result.contains("face_liveness"))
            {
                //                qDebug() << "success";
                video_msg = result.value("face_liveness").toDouble();
                if (video_msg > 0.3)
                {
                    qDebug() << "successful";
                    video_flag = 1;
                }
                else
                {
                    qDebug() << "failed";
                    video_flag = 0;
                }
            }
            //            return video_flag;
        }
    }
}
