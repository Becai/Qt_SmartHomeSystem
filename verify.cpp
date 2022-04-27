#include "verify.h"

#include <QBuffer>
#include <QMessageBox>

#include "logger.h"
#include "ui_verify.h"
Verify::Verify(QWidget *parent) : QWidget(parent), ui(new Ui::Verify) {
  camera = new QCamera(this);
  viewfinder = new QCameraViewfinder(this);
  capture = new QCameraImageCapture(camera);
  camera->setViewfinder(viewfinder);
  ui->setupUi(this);
  ui->camera_layout->addWidget(viewfinder);

  capture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
  camera->setCaptureMode(QCamera::CaptureStillImage);
  connect(capture, &QCameraImageCapture::imageCaptured, this,
          &Verify::capture_pic);
}

Verify::~Verify() {
  delete camera;
  delete viewfinder;
  delete capture;
  delete ui;
}

void Verify::start_capture() { this->camera->start(); }

void Verify::closeEvent(QCloseEvent *) { this->camera->stop(); }

void Verify::capture_pic(int, QImage image) {
  this->camera->stop();
  QByteArray bytes;
  QBuffer buffer(&bytes);
  image.save(&buffer, "jpeg");
  //活体检测
  //将图片编码为BASE64
  QString base64str = QString(bytes.toBase64());
  buffer.close();

  //组装JSON
  QJsonArray JsonArray;
  QJsonObject JsonData;

  JsonData.insert("image", base64str);
  JsonData.insert("image_type", "BASE64");
  JsonData.insert("face_field", "age,beauty,spoofing");
  JsonData.insert("option", "COMMON");

  JsonArray.insert(0, JsonData);
  QString url =
      "https://aip.baidubce.com/rest/2.0/face/v3/faceverify?access_token=" +
      this->face_token;
  Request request;
  QJsonObject response = request.post_array(url, JsonArray);
  qDebug() << QJsonDocument(response).toJson().toStdString().c_str() << endl;
  if (response.isEmpty()) {
    logger_error(LOGGER("request verify fail"));
    QMessageBox::critical(this, "错误", "出现异常");
    this->camera->start();
    return;
  }
  int err_code = response.value("error_code").toInt();
  if (err_code != 0) {
    QString err_msg = response.value("error_msg").toString();
    QMessageBox::warning(this, "警告", err_msg);
    this->camera->start();
    return;
  }
  QJsonObject result = response.value("result").toObject();
  QJsonValue liveness = result.value("face_liveness");
  double score = liveness.toDouble();
  if (score < 0.3) {
    QMessageBox::warning(this, "警告", "认证失败");
    this->camera->start();
    return;
  }
  QMessageBox::information(this, "信息", "认证成功");
  emit verify_ok();
  this->camera->stop();
  this->close();
  return;
}

void Verify::on_cancel_btn_clicked() {
  this->camera->stop();
  this->close();
}

void Verify::on_ok_btn_clicked() { this->capture->capture("./demo.jpg"); }
