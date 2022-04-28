#include "operation.h"

//设备号
const uint8_t DEVICE_VOICE = 0;   // 语音模块
const uint8_t DEVICE_CAMERA = 1;  //相机模块
const uint8_t DEVICE_ZIGBEE = 2;  // m0模块
//语音模块操作
const uint8_t OPERATION_VOICE_PLAY = 0x01;  // 播放语音
//相机模块操作
const uint8_t OPERATION_CAMERA_START = 0x00;    //摄像头开始采集
const uint8_t OPERATION_CAMERA_CAPTURE = 0x01;  //捕获一张图片
const uint8_t OPERATION_CAMERA_STOP = 0x03;     //摄像头停止采集
// zigbee模块操作
const uint8_t OPERATION_ZIGBEE_LIGHT_ON = 0x00;     //开灯
const uint8_t OPERATION_ZIGBEE_LIGHT_OFF = 0x01;    //关灯
const uint8_t OPERATION_ZIGBEE_BUZZER_ON = 0x02;    //开蜂鸣器
const uint8_t OPERATION_ZIGBEE_BUZZER_OFF = 0x03;   //关蜂鸣器
const uint8_t OPERATION_ZIGBEE_FANS_ON = 0x04;      //开风扇
const uint8_t OPERATION_ZIGBEE_FANS_SLOW = 0x05;    //风扇慢速
const uint8_t OPERATION_ZIGBEE_FANS_MID = 0x06;     //风扇中速
const uint8_t OPERATION_ZIGBEE_FANS_FAST = 0x07;    //风扇快速
const uint8_t OPERATION_ZIGBEE_FANS_OFF = 0x08;     //风扇关
const uint8_t OPERATION_ZIGBEE_DIGITAL_ON = 0x09;   //开数码管
const uint8_t OPERATION_ZIGBEE_DIGITAL_OFF = 0x0a;  //关数码管
const uint8_t OPERATION_ZIGBEE_INFO = 0x0b;         //获取环境信息

const size_t buf_size = 1024 * 1024;
Operation::Operation() {
  // 1MB
  this->img_buf = new char[buf_size];
  this->img_buf[0] = 0;
}

Operation::~Operation() { delete[] img_buf; }

void Operation::light_on(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_ZIGBEE;
  buffer[1] = OPERATION_ZIGBEE_LIGHT_ON;
  this->operate(socket, buffer);
}
void Operation::light_off(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_ZIGBEE;
  buffer[1] = OPERATION_ZIGBEE_LIGHT_OFF;
  this->operate(socket, buffer);
}
void Operation::fan_on(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_ZIGBEE;
  buffer[1] = OPERATION_ZIGBEE_FANS_ON;
  this->operate(socket, buffer);
}
void Operation::fan_off(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_ZIGBEE;
  buffer[1] = OPERATION_ZIGBEE_FANS_OFF;
  this->operate(socket, buffer);
}
void Operation::fan_slow(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_ZIGBEE;
  buffer[1] = OPERATION_ZIGBEE_FANS_SLOW;
  this->operate(socket, buffer);
}
void Operation::fan_mid(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_ZIGBEE;
  buffer[1] = OPERATION_ZIGBEE_FANS_MID;
  this->operate(socket, buffer);
}
void Operation::fan_fast(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_ZIGBEE;
  buffer[1] = OPERATION_ZIGBEE_FANS_FAST;
  this->operate(socket, buffer);
}
void Operation::buzzer_on(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_ZIGBEE;
  buffer[1] = OPERATION_ZIGBEE_BUZZER_ON;
  this->operate(socket, buffer);
}
void Operation::buzzer_off(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_ZIGBEE;
  buffer[1] = OPERATION_ZIGBEE_BUZZER_OFF;
  this->operate(socket, buffer);
}
void Operation::digital_on(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_ZIGBEE;
  buffer[1] = OPERATION_ZIGBEE_DIGITAL_ON;
  this->operate(socket, buffer);
}
void Operation::digital_off(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_ZIGBEE;
  buffer[1] = OPERATION_ZIGBEE_DIGITAL_OFF;
  this->operate(socket, buffer);
}
QByteArray Operation::env_info(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_ZIGBEE;
  buffer[1] = OPERATION_ZIGBEE_INFO;
  this->operate(socket, buffer);
  char buf[64];
  buf[0] = 0;
  qint64 code = socket->read(buf, 64);

  buffer.clear();
  if (code <= 0) {
    return buffer;
  }
  if (buf[0] != DEVICE_ZIGBEE) {
    return buffer;
  }
  buffer.append(buf + 2, code - 2);
  return buffer;
}
void Operation::camera_start(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_CAMERA;
  buffer[1] = OPERATION_CAMERA_START;
  this->operate(socket, buffer);
}
void Operation::camera_stop(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_CAMERA;
  buffer[1] = OPERATION_CAMERA_STOP;
  this->operate(socket, buffer);
}
QPixmap Operation::camera_get(QTcpSocket *socket) {
  QByteArray buffer;
  buffer.resize(2);
  buffer[0] = DEVICE_CAMERA;
  buffer[1] = OPERATION_CAMERA_CAPTURE;
  this->operate(socket, buffer);

  qint64 code = socket->read(this->img_buf, buf_size);
  QPixmap img;
  if (code <= 0) {
    return img;
  }
  if (this->img_buf[0] != DEVICE_CAMERA) {
    return img;
  }

  img.loadFromData((const unsigned char *)this->img_buf + 2, code - 2);
  return img;
}

void Operation::voice_play(QTcpSocket *socket, QString msg) {
  QByteArray buffer;
  buffer.resize(3 + msg.size());
  buffer[0] = DEVICE_VOICE;
  buffer[1] = OPERATION_VOICE_PLAY;
  buffer[3] = 0x02;  //内容为utf8编码
  buffer.append(msg.toUtf8());
  this->operate(socket, buffer);
}

void Operation::operate(QTcpSocket *socket, QByteArray body) {
  socket->write(body);
}
