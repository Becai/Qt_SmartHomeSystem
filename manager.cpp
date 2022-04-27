#include "manager.h"

#include <QApplication>
#include <QFile>

#include "logger.h"
Manager::Manager() {}

QString get_token_func(QString api_key, QString secret_key) {
  Request *request = new Request();
  QJsonObject json;
  QString url = "https://aip.baidubce.com/oauth/2.0/token";
  url.append("?grant_type=client_credentials&");
  url.append("client_id=");
  url.append(api_key);
  url.append("&client_secret=");
  url.append(secret_key);
  QJsonObject body = request->post(url, json);
  if (body.isEmpty()) {
    logger_error(LOGGER("post fail"));
    return "";
  }
  QJsonValue token = body.value("access_token");
  return token.toString();
}

void Manager::get_token() {
  QString setting_file = QApplication::applicationDirPath() + "/token.json";
  logger_debug(LOGGER("setting file:%s"), setting_file.toStdString().c_str());
  QJsonObject api_key = this->read_api_key(setting_file);
  if (api_key.isEmpty()) {
    return;
  }
  if (api_key.contains("audio_token")) {
    QJsonValue audio_token = api_key.value("audio_token");
    this->audio_token = audio_token.toString();
  } else {
    QJsonValue audio_api_key = api_key.value("audio_api_key");
    QJsonValue audio_secret_key = api_key.value("audio_secret_key");
    this->audio_token =
        get_token_func(audio_api_key.toString(), audio_secret_key.toString());
  }

  if (api_key.contains("face_token")) {
    QJsonValue face_token = api_key.value("face_token");
    this->face_token = face_token.toString();
  } else {
    QJsonValue face_api_key = api_key.value("face_api_key");
    QJsonValue face_secret_key = api_key.value("face_secret_key");
    this->face_token =
        get_token_func(face_api_key.toString(), face_secret_key.toString());
  }
  api_key.insert("audio_token", this->audio_token);
  api_key.insert("face_token", this->face_token);
  logger_info(LOGGER("audio_token:%s, face_token:%s"),
              this->audio_token.toStdString().c_str(),
              this->face_token.toStdString().c_str());
  QFile file(setting_file);
  bool ok = file.open(QIODevice::WriteOnly);
  if (!ok) {
    logger_error(LOGGER("open file fail"));
    return;
  }
  file.write(QJsonDocument(api_key).toJson());
  file.close();
}

QJsonObject Manager::read_api_key(QString name) {
  QFile file(name);
  bool ok = file.open(QIODevice::ReadOnly);
  QJsonObject empty;
  if (!ok) {
    logger_error(LOGGER("open file fail"));
    return empty;
  }
  QByteArray data = file.readAll();
  QJsonParseError json_error;
  QJsonDocument doc = QJsonDocument::fromJson(data, &json_error);
  if (json_error.error != QJsonParseError::NoError) {
    logger_error(LOGGER("parse json fail"));
    return empty;
  }
  file.close();
  QJsonObject json = doc.object();
  return json;
}
