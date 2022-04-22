#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QTcpSocket>

#include <QStringListModel>
#include <QStandardItemModel>
#include <QString>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void timerUpdate();

    void on_conn_btn_clicked();

    void succ_conn();

private:
    Ui::Widget *ui;
    //客户端套接字
    QTcpSocket *sock_client;
    //连接标志位，0表示未成功
    int conn_flag;
    //
    QStringListModel *Model;
    QStandardItemModel *ItemModel;

};

#endif // WIDGET_H
