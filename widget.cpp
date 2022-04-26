#include "widget.h"
#include "ui_widget.h"

#include "httpermanage.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //定时器：显示当前时间
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
    timer->start(1000);

    //设置标签按钮文本初始内容
    ui->conn_btn->setText("连接系统");
    ui->conn_label->setText("智能家居系统");
    ui->lab_audio->setText("长按按钮说话");
    ui->audio_btn->setText("语音助手");

    //客户端套接字初始化
    sock_client = NULL;
    //连接标志位，0表示未连接
    conn_flag = 0;
    //活体检测标志位初始化
    video_flag = 0;

    //ItemModel初始化
    ItemModel = new QStandardItemModel(this);

    //定义环境信息列表
    QStringList list;
    list.append("温度");
    list.append("湿度");
    list.append("空气质量");

    for(int i=0; i<list.size(); i++)
    {
        QString str = list.at(i);
        QStandardItem *item = new QStandardItem(str);
        //列表字体设置
        QFont font;
        font.setPixelSize(20);
        font.setFamily("SimHei");
        item->setFont(font);
        //设置文本居中对齐
        item->setTextAlignment(Qt::AlignCenter);

        //        item->setEnabled(false);

        ItemModel->appendRow(item);
    }
    ui->listView_eco->setModel(ItemModel);
    //    Model = new QStringListModel(list);
    //    ui->listView_eco->setModel(Model);
    //    ui->listView_eco->setEnabled(false);

    //获取语音识别token值
    http = new httpermanage;
    http->get_token("https://aip.baidubce.com/oauth/2.0/token", "mThPZlSuYCwsddqDs7RmcMC3",
                    "yNZH0G1gcmgLFIlCKdTQN1ZSSqvEUAdD", "audio");
    //获取人脸检测token值
    http->get_token("https://aip.baidubce.com/oauth/2.0/token", "wrr4H2vQthmAAQgGAFOxoRzW",
                    "wtIIuU8xXMfeIM7zvmbHlPzBpC8ymfEW", "video");


    //获取音频设备源文件
    const auto && availableDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    if (!availableDevices.isEmpty())
    {
        m_currentDevice = availableDevices.first();

        QAudioFormat format;
        format.setSampleRate(16000); //设置声音采样频率
        format.setChannelCount(1); //设置声音的声道
        format.setSampleSize(16); //16位深
        format.setSampleType(QAudioFormat::SignedInt);
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setCodec("audio/pcm");

        m_audioInput = new QAudioInput(m_currentDevice, format, this);
    }

}

Widget::~Widget()
{
    delete ui;
}

//实时显示时间
void Widget::timerUpdate()
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    ui->labDateTime->setText(str);
}

//连接服务器按钮
void Widget::on_conn_btn_clicked()
{
    //活体检测
    //将图片编码为BASE64
    QImage image(":/img/test.jpeg");
    QByteArray bytes;
    QBuffer buffer(&bytes);

    image.save(&buffer, "jpeg");
    QString base64str = QString(bytes.toBase64());

    //    FILE *f = fopen("test.txt", "w+");
    //    char *str = base64str.toLatin1().data();
    //    fwrite(str, strlen(str), 1, f);
    //    fclose(f);

    buffer.close();

    //组装JSON
    QJsonArray JsonArray;
    QJsonObject JsonData;

    JsonData.insert("image", base64str);
    JsonData.insert("image_type", "BASE64");
    JsonData.insert("face_field", "age,beauty,spoofing");
    JsonData.insert("option", "COMMON");

    JsonArray.insert(0, JsonData);

    //发送请求
    //    QNetworkAccessManager *manager; //网络访问管理
    //    manager = new QNetworkAccessManager;
    //    connect(manager, SIGNAL(finished(QNetworkReply*)), this,
    //            SLOT(recv_video_msg(QNetworkReply*)));

    qDebug() << http->video_token.toLatin1().data();

    QString Url = "https://aip.baidubce.com/rest/2.0/face/v3/faceverify?access_token=" + http->video_token;

    //    QNetworkRequest request((QUrl(Url)));
    //    request.setHeader(QNetworkRequest::ContentTypeHeader, "ContentType:application/json");

    QJsonDocument doc;
    doc.setArray(JsonArray);
    //    manager->post(request, doc.toJson());

    http->get_msg(Url, doc.toJson(), "video");

    //    FILE *f = fopen("test.txt", "w+");
    //    char *str = doc.toJson().data();
    //    fwrite(str, strlen(str), 1, f);
    //    fclose(f);

    if (video_flag == 1)
    {
        if(conn_flag == 0)
        {
            //            sock_client = NULL;
            if(sock_client == NULL)
            {
                sock_client = new QTcpSocket;
                sock_client->connectToHost("192.168.235.128", 8888);
                //关联连接成功信号与槽函数
                connect(sock_client, SIGNAL(connected()), this, SLOT(succ_conn()));
            }
        }
        else if(conn_flag == 1)
        {
            ui->conn_btn->setText("连接系统");
            sock_client->disconnectFromHost(); //断开与服务器的连接
            sock_client = NULL;
            conn_flag = 0;
        }
    }
    else
    {
        qDebug() << "检测到非人类操作！";
    }

}

//连接成功槽函数
void Widget::succ_conn()
{
    ui->conn_btn->setText("断开连接");
    conn_flag = 1;
}

void Widget::on_audio_btn_pressed()
{
    m_buffer = new QBuffer;
    m_buffer->open(QIODevice::ReadWrite);
    m_audioInput->start(m_buffer);

    ui->lab_audio->setText("松开发送语音");
}

void Widget::on_audio_btn_released()
{
    ui->lab_audio->setText("长按按钮说话");
    m_audioInput->stop();
    const auto &sendData = m_buffer->data();
    m_buffer->deleteLater();

    //组装JSON
    QJsonObject JsonData;
    JsonData.insert("format", "pcm");
    JsonData.insert("rate", "16000");
    JsonData.insert("channel", "1");
    JsonData.insert("cuid", "baidu_workshop");
    JsonData.insert("token", http->audio_token);
    JsonData.insert("dev_pid", "1536");
    JsonData.insert("speech", QString(sendData.toBase64()));
    JsonData.insert("len", sendData.size());

    qDebug() << http->audio_token.toLatin1().data();
    //发送请求
    //    QNetworkAccessManager *my_manager; //网络访问管理
    //    my_manager = new QNetworkAccessManager;
    //    connect(my_manager, SIGNAL(finished(QNetworkReply*)), this,
    //            SLOT(recv_audio_msg(QNetworkReply*)));

    QString Url = "http://vop.baidu.com/server_api";
    //    QNetworkRequest request((QUrl(Url)));
    //    request.setHeader(QNetworkRequest::ContentTypeHeader, "ContentType:application/json");
    //       my_manager->post(request, QJsonDocument(JsonData).toJson());

    http->get_msg(Url, QJsonDocument(JsonData).toJson(), "audio");

}
