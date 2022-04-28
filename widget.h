#ifndef WIDGET_H
#define WIDGET_H

#include <string.h>

#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QBuffer>
#include <QCloseEvent>
#include <QDateTime>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QStandardItemModel>
#include <QString>
#include <QStringListModel>
#include <QTcpSocket>
#include <QTimer>
#include <QWidget>

#include "manager.h"
#include "operation.h"
#include "verify.h"
#include "voice_table.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget {
  Q_OBJECT

 public:
  explicit Widget(QWidget *parent = nullptr);
  void closeEvent(QCloseEvent *);
  ~Widget();

 private slots:
  //更新时间信息
  void timerUpdate();
  //更新监控
  void update_monitor();
  //连接
  void on_conn_btn_clicked();
  //连接成功槽函数
  void on_connect();
  //认证成功
  void on_verify_ok();

  void on_audio_btn_pressed();

  void on_audio_btn_released();

  void on_light_btn_clicked();

  void on_fans_btn_clicked();

  void on_digital_btn_clicked();

  void on_cheese_btn_clicked();

  void on_auto_btn_clicked();

  void on_monitor_btn_clicked();

 private:
  Ui::Widget *ui;
  Operation operation;
  QTimer *timer;
  QTimer *env_timer;
  int env_update_flag;
  //客户端套接字
  QTcpSocket *sock_client;
  //音频设备
  QAudioDeviceInfo m_currentDevice;
  QAudioInput *m_audioInput;
  QBuffer *m_buffer;

  //连接标志位
  bool conn_flag;
  Manager *manager;
  //各个控件的状态
  bool light_status;
  int fan_status;
  bool buzzer_status;
  bool digital_status;
  bool monitor_status;
  bool auto_status;
  //语音助手对话表
  VoiceTable table;
  Verify verify;
  bool verify_status;  //是否已经认证
  bool audio_ok;

 private:
  //信息弹框
  void msg_info(QString msg);
  void msg_warn(QString msg);
  void msg_error(QString msg);
};

#endif  // WIDGET_H
