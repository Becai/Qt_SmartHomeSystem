#ifndef MANAGER_H
#define MANAGER_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>

#include "request.h"
class Manager {
 public:
  Manager();
  QString audio_token;
  QString face_token;

 public:
  void get_token();

 private:
  QJsonObject read_api_key(QString name);
};

#endif  // MANAGER_H
