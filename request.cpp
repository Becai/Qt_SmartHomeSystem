#include "request.h"

#include <QEventLoop>
#include <QJsonParseError>
#include <QObject>

#include "logger.h"

Request::Request() { this->manager = new QNetworkAccessManager; }

// get请求
QJsonObject Request::get(QString url) {
  QNetworkRequest request;
  request.setUrl(QUrl(url));

  QNetworkReply *reply = this->manager->get(request);
  QEventLoop loop;
  QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
  loop.exec(QEventLoop::ExcludeUserInputEvents);  //阻塞

  if (reply->error() != QNetworkReply::NoError) {
    logger_error(LOGGER("get error: %s"), url.toStdString().c_str());
  }

  QByteArray replayData = reply->readAll();
  QJsonParseError json_error;
  QJsonDocument doc = QJsonDocument::fromJson(replayData, &json_error);
  if (json_error.error != QJsonParseError::NoError) {
    logger_error(LOGGER("parse json error"));
  }
  QJsonObject json = doc.object();
  return json;
}

// post 请求
QJsonObject Request::post(QString url, QJsonObject body) {
  QNetworkRequest request;
  request.setUrl(QUrl(url));
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QByteArray data = QJsonDocument(body).toJson();
  QNetworkReply *reply = this->manager->post(request, data);
  QEventLoop loop;
  QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
  loop.exec(QEventLoop::ExcludeUserInputEvents);
  if (reply->error() != QNetworkReply::NoError) {
    logger_error(LOGGER("post error: %s"), url.toStdString().c_str());
  }

  QByteArray replayData = reply->readAll();
  QJsonParseError json_error;
  QJsonDocument doc = QJsonDocument::fromJson(replayData, &json_error);
  if (json_error.error != QJsonParseError::NoError) {
    logger_error(LOGGER("parse json error"));
  }
  QJsonObject json = doc.object();
  return json;
}

QJsonObject Request::post_array(QString url, QJsonArray array) {
  QNetworkRequest request;
  request.setUrl(QUrl(url));
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QByteArray data = QJsonDocument(array).toJson();
  QNetworkReply *reply = this->manager->post(request, data);
  QEventLoop loop;
  QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
  loop.exec(QEventLoop::ExcludeUserInputEvents);
  if (reply->error() != QNetworkReply::NoError) {
    logger_error(LOGGER("post error: %s"), url.toStdString().c_str());
  }

  QByteArray replayData = reply->readAll();
  QJsonParseError json_error;
  QJsonDocument doc = QJsonDocument::fromJson(replayData, &json_error);
  if (json_error.error != QJsonParseError::NoError) {
    logger_error(LOGGER("parse json error"));
  }
  QJsonObject json = doc.object();
  return json;
}
