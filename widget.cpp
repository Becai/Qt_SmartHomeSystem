#include "widget.h"

#include <QIcon>
#include <QMessageBox>

#include "logger.h"
#include "manager.h"
#include "request.h"
#include "ui_widget.h"

#define TEMP_UNIT_CHAR "℃"
#define HUM_UNIT_CHAR "%"
#define ILLU_UNIT_CHAR "lx"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
  ui->setupUi(this);
  ui->labDateTime->setText("时间");
  ui->conn_btn->setText("人脸登录");
  ui->audio_btn->setText("语音助手");
  ui->tem_info->setText("0");
  ui->hum_info->setText("0 %");
  ui->illu_info->setText("0 lx");
  ui->light_btn->setText("开灯");
  ui->fans_btn->setText("开风扇");
  ui->digital_btn->setText("开数码管");
  ui->audio_btn->setText("语音助手");
  ui->cheese_btn->setText("拍照");
  ui->auto_btn->setText("开启自动管理");

  this->ligth_status = false;
  this->fan_status = 0;
  this->buzzer_status = false;
  this->digital_status = false;
  this->monitor_status = 0;
  this->auto_status = false;
  this->verify_status = false;
  //定时器：显示当前时间
  this->timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
  timer->start(1000);

  //和服务端建立链接
  this->sock_client = new QTcpSocket(this);
  //  QString host_name = "localhost";
  //  this->sock_client->connectToHost(host_name, 8888);
  //  connect(this->sock_client, SIGNAL(connected()), this, SLOT());

  //连接标志位，0表示未连接
  this->conn_flag = false;
  this->manager = new Manager;
  manager->get_token();
  if (this->manager->audio_token == "") {
    logger_error(LOGGER("get audio token fail"));
  }
  if (this->manager->face_token == "") {
    logger_error(LOGGER("get face token fail"));
  }

  this->verify.face_token = this->manager->face_token;
  //认证成功信号
  connect(&(this->verify), SIGNAL(verify_ok()), this, SLOT(on_connect()));
  //获取音频设备源文件
  const auto &&availableDevices =
      QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
  if (!availableDevices.isEmpty()) {
    this->m_currentDevice = availableDevices.first();
    QAudioFormat format;
    format.setSampleRate(16000);  //设置声音采样频率
    format.setChannelCount(1);    //设置声音的声道
    format.setSampleSize(16);     // 16位深
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");

    this->m_audioInput = new QAudioInput(m_currentDevice, format, this);
  }
}

Widget::~Widget() {
  delete sock_client;
  delete timer;
  delete m_audioInput;
  delete manager;
  delete ui;
}

//实时显示时间
void Widget::timerUpdate() {
  QDateTime time = QDateTime::currentDateTime();
  QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
  ui->labDateTime->setText(str);
}

//连接服务器按钮
void Widget::on_conn_btn_clicked() {
  if (!this->conn_flag) {
    logger_error(LOGGER("未连接服务器"));
    msg_error("未连接服务器");
    //    return;
  }
  if (this->verify_status) {
    return;
  }
  this->verify.start_capture();
  this->verify.show();
  return;
}

//连接成功槽函数
void Widget::on_connect() {
  logger_debug(LOGGER("成功连接服务端"));
  this->conn_flag = true;
}

//语音助手按钮
void Widget::on_audio_btn_pressed() {
  m_buffer = new QBuffer;
  m_buffer->open(QIODevice::ReadWrite);
  m_audioInput->start(m_buffer);
}

void Widget::on_audio_btn_released() {
  m_audioInput->stop();
  const auto &sendData = m_buffer->data();
  m_buffer->deleteLater();
  //组装JSON
  QJsonObject JsonData;
  JsonData.insert("format", "pcm");
  JsonData.insert("rate", "16000");
  JsonData.insert("channel", "1");
  JsonData.insert("cuid", "baidu_workshop");
  JsonData.insert("token", this->manager->audio_token);
  JsonData.insert("dev_pid", "1536");
  JsonData.insert("speech", QString(sendData.toBase64()));
  JsonData.insert("len", sendData.size());
  QString url = "http://vop.baidu.com/server_api";
  Request request;
  QJsonObject response = request.post(url, JsonData);
  if (response.isEmpty()) {
    logger_error(LOGGER("识别失败"));
    this->msg_error("语音识别失败");
    return;
  }
  QJsonValue err_no = response.value("err_no");
  if (err_no.toInt() != 0) {
    QJsonValue err_msg = response.value("err_msg");
    logger_debug(LOGGER("error msg: %s"),
                 err_msg.toString().toStdString().c_str());
    return;
  }
  QJsonArray result = response.value("result").toArray();
  for (int i = 0; i < result.size(); i++) {
    QJsonValue value = result.at(i);
    logger_debug(LOGGER("%s"), value.toString().toStdString().c_str());
  }
  // todo
}

void Widget::update_monitor() {}

void Widget::msg_error(QString msg) {
  QMessageBox::critical(this, "错误", msg);
}

void Widget::msg_info(QString msg) {
  QMessageBox::information(this, "提示", msg);
}

void Widget::msg_warn(QString msg) { QMessageBox::warning(this, "警告", msg); }

void Widget::on_light_btn_clicked() {
  this->ligth_status = !this->ligth_status;
  if (this->ligth_status) {
    //开灯
    this->ui->light_btn->setText("关灯");
  } else {
    //关灯
    this->ui->light_btn->setText("开灯");
  }
}

void Widget::on_fans_btn_clicked() {
  this->fan_status = (this->fan_status + 1) % 4;
  switch (this->fan_status) {
    case 0:
      this->ui->fans_btn->setText("风扇开");
      break;
    case 1:
      this->ui->fans_btn->setText("风扇中速");
      break;
    case 2:
      this->ui->fans_btn->setText("风扇快速");
      break;
    case 3:
      this->ui->fans_btn->setText("风扇关");
      break;
  }
}

void Widget::on_digital_btn_clicked() {
  this->digital_status = !this->digital_status;
  if (this->digital_status) {
    //开
    this->ui->digital_btn->setText("关数码管");
  } else {
    //关
    this->ui->digital_btn->setText("开数码管");
  }
}

void Widget::on_cheese_btn_clicked() { logger_debug(LOGGER("拍照")); }

void Widget::on_auto_btn_clicked() {
  this->auto_status = !this->auto_status;
  if (this->auto_status) {
    this->ui->auto_btn->setText("关闭自动管理");
  } else {
    this->ui->auto_btn->setText("开启自动管理");
  }
}

void Widget::on_monitor_btn_clicked() {
  bool ok = this == verify.parent();
  qDebug() << verify.parent() << endl;
  qDebug() << ok << endl;
  this->monitor_status = !this->monitor_status;
  if (this->monitor_status) {
    this->ui->monitor_btn->setText("关闭监控");
  } else {
    this->ui->monitor_btn->setText("开启监控");
  }
}

void Widget::closeEvent(QCloseEvent *) {
  this->timer->stop();
  if (this->sock_client != nullptr) {
    this->sock_client->close();
  }
}
