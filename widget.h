#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QTcpSocket>

#include <QStringListModel>
#include <QStandardItemModel>
#include <QString>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>

//#include <QNetworkAccessManager>
//#include <QNetworkRequest>
//#include <QNetworkReply>

#include <QAudioInput>
#include <QAudioDeviceInfo>
#include <QBuffer>

#include <QImage>

#include <string.h>

#include "httpermanage.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void timerUpdate();

    void on_conn_btn_clicked();
    //连接成功槽函数
    void succ_conn();

    void on_audio_btn_pressed();

    void on_audio_btn_released();

private:
    Ui::Widget *ui;
    //客户端套接字
    QTcpSocket *sock_client;
    //连接标志位，0表示未成功
    int conn_flag;
    //环境信息listview
    QStringListModel *Model;
    QStandardItemModel *ItemModel;

    //音频设备
    QAudioDeviceInfo m_currentDevice;
    QAudioInput *m_audioInput;
    QBuffer *m_buffer;

//    QString audio_msg;

    //摄像头
//    double video_msg;
    int video_flag;


    httpermanage * http;
};

#endif // WIDGET_H
