#ifndef VERIFY_H
#define VERIFY_H

#include <QBuffer>
#include <QByteArray>
#include <QCamera>
#include <QCameraImageCapture>
#include <QCameraViewfinder>
#include <QCloseEvent>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QWidget>

#include "request.h"
namespace Ui {
class Verify;
}

class Verify : public QWidget {
  Q_OBJECT

 public:
  QString face_token;

 public:
  explicit Verify(QWidget *parent = nullptr);
  void closeEvent(QCloseEvent *event);
  void start_capture();
  void capture_pic(int, QImage image);
  ~Verify();

 private slots:
  void on_cancel_btn_clicked();

  void on_ok_btn_clicked();

 private:
  Ui::Verify *ui;
  QCamera *camera;
  QCameraViewfinder *viewfinder;
  QCameraImageCapture *capture;
 signals:
  void verify_ok();
};

#endif  // VERIFY_H
