#include "cmdwidget.h"
#include "ui_cmdwidget.h"

CmdWidget::CmdWidget(QWidget *parent) :
    QWidget(parent), addNum(0), allowInput(true),
    ui(new Ui::CmdWidget)
{
    ui->setupUi(this);
    this->setFixedSize(900, 611);
    this->setWindowIcon(QPixmap(":/res/logo.png"));
    this->setWindowTitle("路线查询");

    MyPushButton * retBtn = new MyPushButton(":/res/return.png");
    retBtn->setParent(this);
    MyPushButton * setBtn = new MyPushButton(":/res/set.png");
    setBtn->setParent(this);
    MyPushButton * clearBtn = new MyPushButton(":/res/clear.png");
    clearBtn->setParent(this);
    retBtn->move( this->width() * 0.2 - retBtn->width() * 0.5, this->height() * 0.92 - retBtn->height() * 0.5);
    setBtn->move( this->width() * 0.5 - setBtn->width() * 0.5, this->height() * 0.92 - setBtn->height() * 0.5);
    clearBtn->move( this->width() * 0.8 - clearBtn->width() * 0.5, this->height() * 0.92 - clearBtn->height() * 0.5);

    //返回
    connect(retBtn,&MyPushButton::clicked,[=](){
        retBtn->zoom1();
        retBtn->zoom2();

        QTimer::singleShot(600, this, [=](){
            emit this->cmdWidgetBack();
        });
    });

    connect(clearBtn, &MyPushButton::clicked,[=](){
        clearBtn->zoom1();
        clearBtn->zoom2();

        allowInput = true;
        ui->Input->clear();
        ui->Output->clear();
    });

    ui->Input->setPlaceholderText("请按input.txt格式输入旅客信息:\n"
                                  "起点(string):\n终点(string):\n策略(int):\n开始时间(QDateTime):\n"
                                  "截止时间(QDateTime):");
    connect(setBtn, &MyPushButton::clicked, [=](){
        setBtn->zoom1();
        setBtn->zoom2();

        emit handleInfo();
    });

    connect(this, &CmdWidget::handleInfo, [=](){
        if(allowInput)
        {
            QString  plainStr = ui->Input->toPlainText();
            std::vector<QString> infoList = handleInput(plainStr);

            QDateTime startTime = QDateTime::fromString(infoList[3] + " " + infoList[4], "yyyy-MM-dd hh:mm:ss");
            QDateTime deadline = QDateTime::fromString(infoList[5] + " " + infoList[6], "yyyy-MM-dd hh:mm:ss");
            int strategy = infoList[2].toInt();
            int origin = Schedule::cityToNum(infoList[0]);
            int destination = Schedule::cityToNum(infoList[1]);

            if(origin == -1 || destination == -1)
            {
                QMessageBox::information(this, "Error", QString::fromWCharArray(L"城市名不合法！"));
            }
            else if (origin == destination)
            {
                QMessageBox::information(this, "Error", QString::fromWCharArray(L"起点和终点不能相同！"));
            }
            else if(strategy != minRisk && strategy != minRiskDDL)
            {
                QMessageBox::information(this, "Error", QString::fromWCharArray(L"策略序号错误"));
            }
            else if(strategy == minRiskDDL && startTime >= deadline)
            {
                QMessageBox::information(this, "Error", QString::fromWCharArray(L"截止时间应晚于起始时间！"));
            }
            else
            {
                Traveler traveler(this->addNum, startTime, deadline, strategy, origin, destination);
                travelerList.push_back(traveler);
                this->addNum++;
                if(!traveler.getPlan().size())
                {
                    QMessageBox::information(this, "Error",QString::fromWCharArray(L"无有效路径！"));
                }
                else
                {
                    QString title = QString("No. %1 traveler:").arg(addNum);
                    ui->Output->addItem(title);
                    for(unsigned i = 0; i < traveler.getPlan().size(); ++i)
                    {
                        QString line = num2City(traveler.getPlan()[i].from) +" "+ num2City(traveler.getPlan()[i].to) + " " +
                                traveler.getPlan()[i].num +" "+ traveler.getPlan()[i].begin.toString("hh:mm") + " " +
                                traveler.getPlan()[i].end.toString("hh:mm");
                        switch (traveler.getPlan()[i].vehicle)
                        {
                        case 0:
                            line += " Car"; break;
                        case 1:
                            line += " Train"; break;
                        case 2:
                            line += " Plane"; break;
                        default:
                            qDebug()<<"wrong vehicle num";
                            break;
                        }
                        ui->Output->addItem(line);
                    }
                }
            }
        }
        else
        {
            QMessageBox::information(this, "Error", QString::fromWCharArray(L"规划前清空窗口"));
        }
        allowInput = false;
    });
}

void CmdWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pix;
    if(!pix.load(":/res/cmdwidget.png"))
    {
        qDebug()<<"fail load";
    }
    painter.drawPixmap(0,0,this->width(),this->height(),pix);
}

std::vector<QString> CmdWidget::handleInput (QString str)
{
    int k = 0, left = 0, right = 0;
    str += " ";
    std::vector<QString> infoList(7);
    for(; left < str.size() && right < str.size(); ++right)
    {
        if(str[right] == " ")
        {
            for(int i = left, j = 0; i < right; ++i, ++j)
            {
                infoList[k][j] = str[i];
            }
            ++k;
            left = right + 1;
        }
    }
    return infoList;
}

QString CmdWidget::num2City(int city)
{
    switch (city) {
    case 0:
        return "Beijing";
    case 1:
        return "Shanghai";
    case 2:
        return "Xi'an";
    case 3:
        return "Wuhan";
    case 4:
        return "Qingdao";
    case 5:
        return "Chengdu";
    case 6:
        return "Guangzhou";
    case 7:
        return "Guiyang";
    case 8:
        return "Lasa";
    case 9:
        return "Wulumuqi";
    case 10:
        return "Xining";
    case 11:
        return "Huhehaote";
    case 12:
        return "Jilin";
    case 13:
        return "Suifenhe";
    case 14:
        return "Heihe";
    default:
        qDebug()<<"wrong num";
        return "";
    }
}

CmdWidget::~CmdWidget()
{
    delete ui;
}
