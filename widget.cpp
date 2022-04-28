#include "widget.h"

#include <synchapi.h>

#include <QIcon>
#include <QMessageBox>
#include <sstream>
#include <string>

#include "logger.h"
#include "manager.h"
#include "request.h"
#include "ui_widget.h"
#define TEMP_UNIT_CHAR "℃"
#define HUM_UNIT_CHAR "%"
#define ILLU_UNIT_CHAR "lx"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
  ui->setupUi(this);
  this->setWindowIcon(QIcon(":/icon/icon/icon.png"));

  ui->labDateTime->setText("时间");
  ui->conn_btn->setText("人脸登录");
  ui->conn_btn->setIcon(QIcon(":/icon/icon/connnect.png"));
  ui->conn_btn->setIconSize(QSize(48, 48));

  ui->audio_btn->setText("语音助手");
  ui->audio_btn->setIcon(QIcon(":/icon/icon/audio.png"));
  ui->audio_btn->setIconSize(QSize(48, 48));

  ui->tem_info->setText("0");
  ui->hum_info->setText("0 %");
  ui->illu_info->setText("0 lx");

  ui->light_btn->setText("开灯");
  ui->light_btn->setIcon(QIcon(":/icon/icon/light.png"));
  ui->light_btn->setIconSize(QSize(48, 48));

  ui->fans_btn->setText("开风扇");
  ui->fans_btn->setIcon(QIcon(":/icon/icon/fan.png"));
  ui->fans_btn->setIconSize(QSize(48, 48));

  ui->digital_btn->setText("开数码管");
  ui->digital_btn->setIcon(QIcon(":/icon/icon/digital.png"));
  ui->digital_btn->setIconSize(QSize(48, 48));

  ui->cheese_btn->setText("拍照");
  ui->cheese_btn->setIcon(QIcon(":/icon/icon/camera.png"));
  ui->cheese_btn->setIconSize(QSize(48, 48));

  ui->auto_btn->setText("开警报");
  ui->auto_btn->setIcon(QIcon(":/icon/icon/alert.png"));
  ui->auto_btn->setIconSize(QSize(48, 48));

  ui->monitor_btn->setIcon(QIcon(":/icon/icon/monitor.png"));
  ui->monitor_btn->setIconSize(QSize(48, 48));

  this->light_status = false;
  this->fan_status = 0;
  this->buzzer_status = false;
  this->digital_status = false;
  this->monitor_status = false;
  this->auto_status = false;
  this->verify_status = false;
  this->env_update_flag = 0;
  this->audio_ok = false;
  //定时器：显示当前时间
  this->timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
  timer->start(1000);

  this->env_timer = new QTimer(this);
  connect(this->env_timer, SIGNAL(timeout()), this, SLOT(update_monitor()));
  // this->env_timer->start((100));

  //和服务端建立链接
  this->sock_client = new QTcpSocket(this);
  //  QString host_name = "169.254.141.8";
  QString host_name = "172.29.189.3";
  this->sock_client->connectToHost(host_name, 8888);
  connect(this->sock_client, SIGNAL(connected()), this, SLOT(on_connect()));

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
  connect(&(this->verify), SIGNAL(verify_ok()), this, SLOT(on_verify_ok()));
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
  delete env_timer;
  delete sock_client;
  delete timer;
  delete m_audioInput;
  delete manager;
  delete ui;
}

void Widget::on_verify_ok() {
  logger_debug(LOGGER("认证成功"));
  this->verify_status = true;
  this->env_timer->start(100);
  this->operation.voice_play(this->sock_client, "欢迎回来，主人");
  this->ui->conn_btn->setText("欢迎回来");
}

//实时显示时间
void Widget::timerUpdate() {
  QDateTime time = QDateTime::currentDateTime();
  QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
  ui->labDateTime->setText(str);
}

//人脸认证
void Widget::on_conn_btn_clicked() {
  if (!this->conn_flag) {
    logger_error(LOGGER("未连接服务器"));
    msg_error("未连接服务器");
    return;
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
  if (!this->conn_flag || !this->verify_status) {
    msg_error("未连接服务器");
    return;
  }
  m_buffer = new QBuffer;
  m_buffer->open(QIODevice::ReadWrite);
  m_audioInput->start(m_buffer);
  this->audio_ok = true;
}

void Widget::on_audio_btn_released() {
  if (!audio_ok) {
    return;
  }
  audio_ok = false;
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
  // todo
  QString value = result.at(0).toString();
  qDebug() << value.toStdString().c_str() << endl;
  QString table_value = this->table.query(value);
  if (!table_value.isEmpty()) {
    this->operation.voice_play(this->sock_client, table_value);
    logger_debug(LOGGER("voice play:%s"), table_value.toStdString().c_str());
    return;
  }
  if (value.contains("开灯")) {
    this->operation.light_on(this->sock_client);
    this->light_status = true;
  } else if (value.contains("关灯")) {
    this->operation.light_off(this->sock_client);
    this->light_status = false;
  } else if (value.contains("开风扇")) {
    this->operation.fan_on(this->sock_client);
    this->fan_status = 1;
  } else if (value.contains("关风扇")) {
    this->operation.fan_off(this->sock_client);
    this->fan_status = 0;
  } else if (value.contains("开数码管")) {
    this->operation.digital_on(this->sock_client);
    this->digital_status = true;
  } else if (value.contains("关数码管")) {
    this->operation.digital_off(this->sock_client);
    this->digital_status = false;
  } else if (value.contains("开警报")) {
    this->operation.buzzer_on(this->sock_client);
  } else if (value.contains("关警报")) {
    this->operation.buzzer_off(this->sock_client);
  } else if (!value.isEmpty()) {
    this->operation.voice_play(this->sock_client, "我没听懂哦");
  }
}

void Widget::update_monitor() {
  if (this->monitor_status) {
    QPixmap pic = this->operation.camera_get(this->sock_client);
    if (pic.isNull()) {
      logger_warn(LOGGER("pic info is empty"));
    } else {
      //防止图片撑大控件
      int h = this->ui->monitor_view->height() - 10;
      int w = this->ui->monitor_view->width() - 10;
      pic = pic.scaled(w, h);
      this->ui->monitor_view->setPixmap(pic);
    }
  }
  if (this->env_update_flag != 0) {
    this->env_update_flag++;
    this->env_update_flag %= 10;
    return;
  }
  this->env_update_flag++;
  //更新环境信息
  QByteArray res = this->operation.env_info(this->sock_client);
  if (res.size() < 36) {
    logger_warn(LOGGER("env info is empty"));
    return;
  }

  unsigned int tem = (unsigned char)res.at(5);
  unsigned int tem1 = (unsigned char)res.at(4);
  QString tem_text;
  tem_text = tem_text.asprintf("%u.%u ℃", tem, tem1);
  this->ui->tem_info->setText(tem_text);
  //  if (this->auto_status && tem >= 26) {
  //    const char data[2] = {0x02, 0x04};
  //    this->sock_client->write(data, 2);
  //  }

  unsigned int hum = (unsigned char)res.at(7) + (unsigned char)res.at(6);
  QString hum_text;
  hum_text = hum_text.asprintf("%u %%", hum);
  this->ui->hum_info->setText(hum_text);

  unsigned int light = (unsigned char)res.at(23) << 24;
  light |= (unsigned char)res.at(22) << 16;
  light |= (unsigned char)res.at(21) << 8;
  light |= (unsigned char)res.at(20);
  QString light_text;
  light_text = light_text.asprintf("%u lx", light);
  this->ui->illu_info->setText(light_text);
  //  if (this->auto_status && light <= 200) {
  //    const char data[2] = {0x02, 0x00};
  //    this->sock_client->write(data, 2);
  //  }
}

void Widget::msg_error(QString msg) {
  QMessageBox::critical(this, "错误", msg);
}

void Widget::msg_info(QString msg) {
  QMessageBox::information(this, "提示", msg);
}

void Widget::msg_warn(QString msg) { QMessageBox::warning(this, "警告", msg); }

void Widget::on_light_btn_clicked() {
  if (!this->conn_flag || !this->verify_status) {
    msg_error("未连接服务器");
    return;
  }
  this->light_status = !this->light_status;
  if (this->light_status) {
    //开灯
    this->ui->light_btn->setText("关灯");
    this->operation.light_on(this->sock_client);
  } else {
    //关灯
    this->ui->light_btn->setText("开灯");
    this->operation.light_off(this->sock_client);
  }
}

void Widget::on_fans_btn_clicked() {
  if (!this->conn_flag || !this->verify_status) {
    msg_error("未连接服务器");
    return;
  }
  this->fan_status = (this->fan_status + 1) % 4;
  switch (this->fan_status) {
    case 0:
      this->ui->fans_btn->setText("风扇开");
      this->operation.fan_off(this->sock_client);
      break;
    case 1:
      this->ui->fans_btn->setText("风扇中速");
      this->operation.fan_on(this->sock_client);
      break;
    case 2:
      this->ui->fans_btn->setText("风扇快速");
      this->operation.fan_mid(this->sock_client);
      break;
    case 3:
      this->ui->fans_btn->setText("风扇关");
      this->operation.fan_fast(this->sock_client);
      break;
  }
}

void Widget::on_digital_btn_clicked() {
  if (!this->conn_flag || !this->verify_status) {
    msg_error("未连接服务器");
    return;
  }
  this->digital_status = !this->digital_status;
  if (this->digital_status) {
    //开
    this->ui->digital_btn->setText("关数码管");
    this->operation.digital_on(this->sock_client);
  } else {
    //关
    this->ui->digital_btn->setText("开数码管");
    this->operation.digital_off(this->sock_client);
  }
}

void Widget::on_cheese_btn_clicked() {
  if (!this->conn_flag || !this->verify_status) {
    msg_error("未连接服务器");
    return;
  }
  logger_debug(LOGGER("拍照"));
  if (this->monitor_status) {
    const QPixmap *pic = this->ui->monitor_view->pixmap();
    pic->save("pic.jpg");
    msg_info("拍照成功");
  } else {
    QPixmap pic = this->operation.camera_get(this->sock_client);
    if (pic.isNull()) {
      msg_error("拍照失败");
      return;
    }
    pic.save("pic.jpg");
  }
  this->operation.voice_play(this->sock_client, "拍好了捏");
}

void Widget::on_auto_btn_clicked() {
  if (!this->conn_flag || !this->verify_status) {
    msg_error("未连接服务器");
    return;
  }
  this->auto_status = !this->auto_status;
  if (this->auto_status) {
    this->ui->auto_btn->setText("关警报");
    //    this->ui->light_btn->setDisabled(true);
    //    this->ui->fans_btn->setDisabled(true);
    //    this->ui->digital_btn->setDisabled(true);
    this->operation.buzzer_on(this->sock_client);
  } else {
    this->ui->auto_btn->setText("开警报");
    //    this->ui->light_btn->setDisabled(false);
    //    this->ui->fans_btn->setDisabled(false);
    //    this->ui->digital_btn->setDisabled(false);
    this->operation.buzzer_off(this->sock_client);
  }
}

void Widget::on_monitor_btn_clicked() {
  if (!this->conn_flag || !this->verify_status) {
    msg_error("未连接服务器");
    return;
  }
  bool ok = this == verify.parent();
  qDebug() << verify.parent() << endl;
  qDebug() << ok << endl;
  this->monitor_status = !this->monitor_status;
  if (this->monitor_status) {
    this->ui->monitor_btn->setText("关闭监控");
    //
  } else {
    this->ui->monitor_btn->setText("开启监控");
  }
}

void Widget::closeEvent(QCloseEvent *) {
  this->timer->stop();
  if (this->sock_client != nullptr) {
    this->sock_client->close();
  }
  this->env_timer->stop();
}
