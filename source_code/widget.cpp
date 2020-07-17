#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    QPixmap pixmap(":/res/logo.png");
    this->setWindowIcon(pixmap);
    this->setFixedSize(900,800);
    this->setWindowTitle("低风险旅行计划");

    MyPushButton * cmdBtn = new MyPushButton(":/res/cmd.png");
    MyPushButton * figBtn = new MyPushButton(":/res/fig.png");
    MyPushButton * exitBtn = new MyPushButton(":/res/exit.png");
    cmdBtn->move( this->width() * 0.2- cmdBtn->width() * 0.5,this->height() * 0.87 - cmdBtn->height() * 0.4);
    cmdBtn->setParent(this);
    figBtn->move( this->width() * 0.5- figBtn->width() * 0.5,this->height() * 0.87 - figBtn->height() * 0.4);
    figBtn->setParent(this);
    exitBtn->move( this->width() * 0.8 - exitBtn->width() * 0.5,this->height() * 0.87 - exitBtn->height() * 0.4);
    exitBtn->setParent(this);

    cmdWidget = new CmdWidget;
    figWidget = new FigWidget;

    connect(figWidget, &FigWidget::figWidgetBack, [=](){
        figWidget->hide();
        this->show();
    });

    connect(cmdWidget, &CmdWidget::cmdWidgetBack, [=](){
        cmdWidget->hide();
        this->show();
    });

    connect(cmdBtn, &QPushButton::clicked, [=](){
        cmdBtn->zoom1();
        cmdBtn->zoom2();

        QTimer::singleShot(500, this, [=](){
            this->hide();
            cmdWidget->show();
        });
    });
    connect(figBtn, &QPushButton::clicked, [=](){
        figBtn->zoom1();
        figBtn->zoom2();

        QTimer::singleShot(500, this, [=](){
            this->hide();
            figWidget->show();
        });
    });
    connect(exitBtn, &QPushButton::clicked, [=](){
        exitBtn->zoom1();
        exitBtn->zoom2();

        QTimer::singleShot(500, this, [=](){
            this->close();
        });
    });
}

//绘图
void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/welcome.png");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);
}

Widget::~Widget()
{
    delete ui;
}

