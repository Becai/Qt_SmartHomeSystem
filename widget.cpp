#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //定时器：显示当前时间
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
    timer->start(1000);

    //设置标签按钮文本初始内容
    ui->conn_btn->setText("连接系统");
    ui->conn_label->setText("智能家居系统");
    //客户端套接字初始化
    sock_client = NULL;
    //连接标志位，0表示未连接
    conn_flag = 0;

    //ItemModel初始化
    ItemModel = new QStandardItemModel(this);

    //定义环境信息列表
    QStringList list;
    list.append("温度");
    list.append("湿度");
    list.append("空气质量");


    for(int i=0; i<list.size(); i++)
    {
        QString str = list.at(i);
        QStandardItem *item = new QStandardItem(str);
        //列表字体设置
        QFont font;
        font.setPixelSize(20);
        font.setFamily("SimHei");
        item->setFont(font);
        //设置文本居中对齐
        item->setTextAlignment(Qt::AlignCenter);

//        item->setEnabled(false);

        ItemModel->appendRow(item);
    }
    ui->listView_eco->setModel(ItemModel);
//    Model = new QStringListModel(list);
//    ui->listView_eco->setModel(Model);
//    ui->listView_eco->setEnabled(false);

}

Widget::~Widget()
{
    delete ui;
}

//实时显示时间
void Widget::timerUpdate()
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    ui->labDateTime->setText(str);
}

void Widget::on_conn_btn_clicked()
{
    if(conn_flag == 0)
    {
//        sock_client = NULL;
        if(sock_client == NULL)
        {
            sock_client = new QTcpSocket;
            sock_client->connectToHost("192.168.235.128", 8888);
            //关联连接成功信号与槽函数
            connect(sock_client, SIGNAL(connected()), this, SLOT(succ_conn()));
        }
    }
    else if(conn_flag == 1)
    {
        ui->conn_btn->setText("连接系统");
        sock_client->disconnectFromHost(); //断开与服务器的连接
        sock_client = NULL;
        conn_flag = 0;
    }
}

//连接成功槽函数
void Widget::succ_conn()
{
    ui->conn_btn->setText("断开连接");
    conn_flag = 1;
}
