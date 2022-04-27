#ifndef OPERATION_H
#define OPERATION_H
#include <QByteArray>
#include <QTcpSocket>

//设备号
extern const uint8_t DEVICE_VOICE;   // 语音模块
extern const uint8_t DEVICE_CAMERA;  //相机模块
extern const uint8_t DEVICE_ZIGBEE;  // m0模块
//语音模块操作
extern const uint8_t OPERATION_VOICE_PLAY;  // 播放语音
//相机模块操作
extern const uint8_t OPERATION_CAMERA_START;    //摄像头开始采集
extern const uint8_t OPERATION_CAMERA_CAPTURE;  //捕获一张图片
extern const uint8_t OPERATION_CAMERA_STOP;     //摄像头停止采集
// zigbee模块操作
extern const uint8_t OPERATION_ZIGBEE_LIGHT_ON;     //开灯
extern const uint8_t OPERATION_ZIGBEE_LIGHT_OFF;    //关灯
extern const uint8_t OPERATION_ZIGBEE_BUZZER_ON;    //开蜂鸣器
extern const uint8_t OPERATION_ZIGBEE_BUZZER_OFF;   //关蜂鸣器
extern const uint8_t OPERATION_ZIGBEE_FANS_ON;      //开风扇
extern const uint8_t OPERATION_ZIGBEE_FANS_SLOW;    //风扇慢速
extern const uint8_t OPERATION_ZIGBEE_FANS_MID;     //风扇中速
extern const uint8_t OPERATION_ZIGBEE_FANS_FAST;    //风扇快速
extern const uint8_t OPERATION_ZIGBEE_FANS_OFF;     //风扇关
extern const uint8_t OPERATION_ZIGBEE_DIGITAL_ON;   //开数码管
extern const uint8_t OPERATION_ZIGBEE_DIGITAL_OFF;  //关数码管
extern const uint8_t OPERATION_ZIGBEE_INFO;         //获取环境信息

class Operation {
 public:
  Operation();
  void light_on(QTcpSocket*);
  void light_off(QTcpSocket*);

  void fan_on(QTcpSocket*);
  void fan_off(QTcpSocket*);
  void fan_slow(QTcpSocket*);
  void fan_mid(QTcpSocket*);
  void fan_fast(QTcpSocket*);

  void buzzer_on(QTcpSocket*);
  void buzzer_off(QTcpSocket*);

  void digital_on(QTcpSocket*);
  void digital_off(QTcpSocket*);

  QByteArray env_info(QTcpSocket*);

  void camera_start(QTcpSocket*);
  void camera_stop(QTcpSocket*);

  QByteArray camera_get(QTcpSocket*);

  void voice_play(QTcpSocket*, QString);

 private:
  void operate(QTcpSocket*, QByteArray body);
};

#endif  // OPERATION_H
