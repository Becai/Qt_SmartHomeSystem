#ifndef REQUEST_H
#define REQUEST_H
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class Request {
 public:
  Request();
  QNetworkAccessManager *manager;

 public:
  QJsonObject get(QString url);
  QJsonObject post(QString url, QJsonObject body);
};

#endif  // REQUEST_H
