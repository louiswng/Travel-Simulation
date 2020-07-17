#include "figwidget.h"
#include "ui_figwidget.h"

FigWidget::FigWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::FigWidget),
     currentTraveler(-1), strategyBtnIdx(minRisk), addtravelertimes(0), startclickedtimes(0)
{
    ui->setupUi(this);
    initFigureUI();
    initTimeThread();
    initFigureConnect();
}

void FigWidget::initFigureUI()
{
    this->setWindowIcon(QPixmap(":/res/logo.png"));
    this->setFixedSize(1103, 713);
    this->setWindowTitle("动画演示");
    ui->StartDateTimeEdit->setEnabled(false);
    ui->DeadlineDateTimeEdit->setEnabled(false);
    ui->StartButton->setEnabled(false);
    ui->DurationText->setEnabled(false);
    ui->CurTimeText->setEnabled(false);
    ui->FareEdit->setEnabled(false);
    ui->TotalTimeEdit->setEnabled(false);

    ui->StrategyChooseBox->setEnabled(false);
    ui->StrategyBtn1->setEnabled(false);
    ui->StrategyBtn2->setEnabled(false);

    ui->StartDateTimeEdit->setDateTime(QDateTime::currentDateTime());
    int deaDay = QDateTime::currentDateTime().date().day();
    deaDay += 5;
    QDateTime deadlineDateTime;
    deadlineDateTime.setDate(QDate(QDateTime::currentDateTime().date().year(), QDateTime::currentDateTime().date().month(), deaDay));
    deadlineDateTime.setTime(QTime(QDateTime::currentDateTime().time()));
    ui->DeadlineDateTimeEdit->setDateTime(deadlineDateTime);
    ui->StartComboBox->setEnabled(false);
    ui->DestinationComboBox->setEnabled(false);
    ui->DestinationComboBox->setCurrentIndex(1);
    ui->TravelerComboBox->setEnabled(false);
}

void FigWidget::initFigureConnect()
{
   connect(ui->addTravelerButton, &QPushButton::clicked, this, &FigWidget::addTravelerButtonClicked);
    connect(ui->StartButton, &QPushButton::clicked, this, &FigWidget::startButtonClicked);

    connect(ui->StrategyBtn1, &QPushButton::toggled, [=](){
        strategyBtnIdx = minRisk;
    });
    connect(ui->StrategyBtn2, &QPushButton::toggled, [=](){
        strategyBtnIdx = minRiskDDL;
    });

    connect(ui->figBackBtn, &QPushButton::clicked,[=](){
        QTimer::singleShot(500, this, [=](){
            emit this->figWidgetBack();
        });
    });

    connect(ui->exitBtn, &QPushButton::clicked, [=](){
        this->close();
    });
    void(QComboBox::*IndexSignal)(int) = &QComboBox::currentIndexChanged;
    connect(ui->TravelerComboBox, IndexSignal, this, &FigWidget::travelerChanged);
    connect(ui->StartButton, &QPushButton::clicked, this, &FigWidget::timeStart);
    connect(mstimer, &QTimer::timeout, this, &FigWidget::displaySpentTime);
    connect(this, SIGNAL(DoStartTimer()), mstimer, SLOT(start()));
}

void FigWidget::initTimeThread()
{
    mstimer = new QTimer;
    mstimer->setInterval(1);

    timethread = new QThread();
    timethread->start();

    mstimer->moveToThread(timethread);
}

void FigWidget::addTravelerButtonClicked()
{
    startclicked.push_back(false);
    addtravelertimes += 1;
    startclickedtimes = 0;

    ui->TravelerComboBox->addItem(QString::number(addtravelertimes));
    ui->TravelerComboBox->setCurrentText(QString::number(addtravelertimes));

    ui->StartButton->setText(QString::fromWCharArray(L"开始"));
    ui->TravelerComboBox->setEnabled(true);
    ui->StartComboBox->setEnabled(true);
    ui->DestinationComboBox->setEnabled(true);
    ui->StartComboBox->setCurrentIndex(0);
    ui->DestinationComboBox->setCurrentIndex(1);
    ui->DeadlineDateTimeEdit->setEnabled(true);
    ui->StartDateTimeEdit->setEnabled(true);
    ui->StartButton->setEnabled(true);

    ui->StrategyChooseBox->setEnabled(true);
    ui->StrategyBtn1->setChecked(true);
    ui->StrategyBtn1->setEnabled(true);
    ui->StrategyBtn2->setEnabled(true);

    ui->StartDateTimeEdit->setDateTime(QDateTime::currentDateTime());
    int deaDay = QDateTime::currentDateTime().date().day();
    deaDay += 5;
    QDateTime deadlineDateTime;
    deadlineDateTime.setDate(QDate(QDateTime::currentDateTime().date().year(), QDateTime::currentDateTime().date().month(), deaDay));
    deadlineDateTime.setTime(QTime(QDateTime::currentDateTime().time()));
    ui->DeadlineDateTimeEdit->setDateTime(deadlineDateTime);
}

void FigWidget::startButtonClicked()
{
    int id = ui->TravelerComboBox->currentIndex();
    if (startclicked[id] == false)
    {
        strategy = getStrategy();
        start = getStart();
        destination = getDestination();

        if (start == destination)
        {
            QMessageBox::information(this, "Error", QString::fromWCharArray(L"出发地和目的地相同"));
            return;
        }

        if (!(ui->StartDateTimeEdit->dateTime() < ui->DeadlineDateTimeEdit->dateTime()))
        {
            QMessageBox::information(this, "Error", QString::fromWCharArray(L"截止时间不应早于当前时间"));
            return;
        }
         Traveler traveler(addtravelertimes-1, getStartTime(),getDeadline(), strategy, start, destination);
         travelerList.push_back(traveler);

        auto plan = travelerList[id].getPlan();
        if (plan.size() == 0)
        {
            QMessageBox::information(this, "Error", QString::fromWCharArray(L"无有效路径"));
            startclicked[id] = false;
            return;
        }
        startclicked[id] = true;
        currentTraveler = id;

        displayTotalTime();
        displayFare(plan);
        displayPath(plan);

        ui->StartButton->setEnabled(false);
        ui->StartComboBox->setEnabled(false);
        ui->DestinationComboBox->setEnabled(false);
        ui->StrategyChooseBox->setEnabled(false);//debug
        ui->DeadlineDateTimeEdit->setEnabled(false);
        ui->StartDateTimeEdit->setEnabled(false);
        ui->StrategyChooseBox->setEnabled(true);
        ui->StrategyBtn1->setEnabled(true);
        ui->StrategyBtn2->setEnabled(true);

        startclickedtimes += 1;
        return;
    }
}

int FigWidget::getStrategy()
{
    return strategyBtnIdx;
}

int FigWidget::getStart()
{
    return ui->StartComboBox->currentIndex();
}

int FigWidget::getDestination()
{
    return ui->DestinationComboBox->currentIndex();
}

QDateTime FigWidget::getDeadline()
{
    return ui->DeadlineDateTimeEdit->dateTime();
}

QDateTime FigWidget::getStartTime()
{
    return ui->StartDateTimeEdit->dateTime();
}

QDateTime FigWidget::getSpentTime()
{
    int id = ui->TravelerComboBox->currentIndex();
    QDateTime usedDateTime = travelerList[id].usedTime;

    int durday = usedDateTime.date().day();
    int durhour = usedDateTime.time().hour();
    int durmin = usedDateTime.time().minute();
    int dursec = usedDateTime.time().second();
    int durmsec = usedDateTime.time().msec();

    switch (ui->spinAndSlider->slider->value())
    {
    case 1:
        durmsec += 360; break;
    case 2:
        durmsec += 720; break;
    case 3:
        durmsec += 1080; break;
    case 4:
        durmsec += 1440; break;
    case 5:
        durmsec += 1800; break;
    case 6:
        durmsec += 2160; break;
    case 7:
        durmsec += 2520; break;
    case 8:
        durmsec += 2880; break;
    case 9:
        durmsec += 3240; break;
    case 10:
        durmsec += 3600; break;
    }

    dursec += durmsec / 1000;
    durmsec = durmsec % 1000;
    durmin += dursec / 60;
    dursec = dursec % 60;
    durhour += durmin / 60;
    durmin = durmin % 60;
    durday += durhour /24;
    durhour = durhour % 24;
    durday = durday % 30;

   travelerList[id].usedTime = QDateTime(QDate(1, 1, durday),
                    QTime(durhour, durmin, dursec, durmsec));
   return travelerList[id].usedTime;
}

void FigWidget::travelerChanged()
{
    int id=ui->TravelerComboBox->currentIndex();
    if (startclicked[id])
    {
        currentTraveler = id;
        ui->StartButton->setEnabled(false);
        ui->StartDateTimeEdit->setDateTime(travelerList[id].startTime);
        int deaDay = travelerList[id].deadlineTime.date().day();
        QDateTime deadlineDateTime;
        deadlineDateTime.setDate(QDate(travelerList[id].deadlineTime.date().year(), travelerList[id].deadlineTime.date().month(), deaDay));
        deadlineDateTime.setTime(QTime(travelerList[id].deadlineTime.time()));
        ui->DeadlineDateTimeEdit->setDateTime(deadlineDateTime);

        if(travelerList[id].strategy == minRisk)
        {
            ui->StrategyBtn1->setEnabled(true);
            ui->StrategyBtn2->setEnabled(false);
        }
        else if(travelerList[id].strategy == minRiskDDL)
        {
            ui->StrategyBtn2->setEnabled(true);
            ui->StrategyBtn1->setEnabled(false);
        }

        ui->StartComboBox->setCurrentIndex(travelerList[id].source);
        ui->DestinationComboBox->setCurrentIndex(travelerList[id].destination);

        displayFare(travelerList[id].getPlan());
        displayTotalTime();
        displayPath(travelerList[id].getPlan());
        displaySpentTime();

        ui->StartComboBox->setEnabled(false);
        ui->DestinationComboBox->setEnabled(false);
        ui->StartDateTimeEdit->setEnabled(false);
        ui->DeadlineDateTimeEdit->setEnabled(false);

    }
    else
    {
        ui->StartButton->setEnabled(true);
        ui->StartButton->setText(QString::fromWCharArray(L"开始"));
        ui->StartComboBox->setEnabled(true);
        ui->DestinationComboBox->setEnabled(true);
        ui->StartDateTimeEdit->setEnabled(true);
        ui->DeadlineDateTimeEdit->setEnabled(true);

        ui->StrategyChooseBox->setEnabled(true);
        ui->StrategyBtn1->setEnabled(true);
        ui->StrategyBtn2->setEnabled(true);

        ui->StartDateTimeEdit->setDateTime(QDateTime::currentDateTime());
        int deaDay = QDateTime::currentDateTime().date().day();
        deaDay += 1;
        QDateTime deadlineDateTime;
        deadlineDateTime.setDate(QDate(QDateTime::currentDateTime().date().year(), QDateTime::currentDateTime().date().month(), deaDay));
        deadlineDateTime.setTime(QTime(QDateTime::currentDateTime().time()));
        ui->DeadlineDateTimeEdit->setDateTime(deadlineDateTime);
        ui->StartComboBox->setCurrentIndex(0);
        ui->DestinationComboBox->setCurrentIndex(1);
        QVBoxLayout *listlayout = new QVBoxLayout;
        QWidget *containwidget = new QWidget(ui->PathList);
        containwidget->setLayout(listlayout);
        ui->PathList->setWidget(containwidget);
        ui->FareEdit->clear();
        ui->TotalTimeEdit->clear();
        ui->DurationText->clear();
        ui->CurTimeText->clear();
        ui->progressBar->setValue(0);

        currentTraveler = -1;
    }
}

void FigWidget::timeStart()
{
    if (mstimer->isActive())
        return;
    emit DoStartTimer();
}

void FigWidget::displaySpentTime()
{
    int id = ui->TravelerComboBox->currentIndex();
    if (startclicked[id])
    {
        QDateTime spentTime = getSpentTime();
        QDateTime totalTime = travelerList[id].totalTime;
        int spentSec = (spentTime.date().day()-1)*24*60*60 + spentTime.time().hour()*60*60 + spentTime.time().minute()*60;
        QDateTime currentTime = travelerList[id].startTime.addSecs(spentSec);

        if (spentTime <= travelerList[id].totalTime)
        {
            ui->DurationText->setText(QString::number(spentTime.date().day()-1) + QString::fromWCharArray(L"天 ")
                    + QString::number(spentTime.time().hour()) + QString::fromWCharArray(L"小时 ")
                    + QString::number(spentTime.time().minute()) + QString::fromWCharArray(L"分钟"));
            ui->CurTimeText->setText(QString::number(currentTime.date().year()) +"/"+QString::number(currentTime.date().month()) +"/"+
                                     QString::number(currentTime.date().day()-1) + " "
                                     + QString::number(currentTime.time().hour()) + QString::fromWCharArray(L":")
                                     + QString::number(currentTime.time().minute()));

            double spentMin = spentSec/60;
            double totalMin = (totalTime.date().day()-1)*24*60 + totalTime.time().hour()*60 + totalTime.time().minute();
            double rate = spentMin/totalMin;
            ui->progressBar->setValue(rate*100);
        }
        else
        {
            ui->progressBar->setValue(100);
            ui->DurationText->setText(QString::number(travelerList[id].totalTime.date().day()-1)
                    + QString::fromWCharArray(L"天 ") +
                    QString::number(travelerList[id].totalTime.time().hour())
                    + QString::fromWCharArray(L"小时 ") +
                    QString::number(travelerList[id].totalTime.time().minute())
                    + QString::fromWCharArray(L"分钟"));

            ui->CurTimeText->setText(QString::number(travelerList[id].totalTime.date().day()-1)
                    + QString::fromWCharArray(L"日 ") +
                    QString::number(travelerList[id].totalTime.time().hour())
                    + QString::fromWCharArray(L": ") +
                    QString::number(travelerList[id].totalTime.time().minute())
                    + QString::fromWCharArray(L":00"));

            qDebug() << "SpentTime: " << QString::number(travelerList[id].totalTime.date().day()-1)
                    + "Day " +
                    QString::number(travelerList[id].totalTime.time().hour())
                    + "Hour " +
                    QString::number(travelerList[id].totalTime.time().minute())
                    + "Minute";
            ui->StartButton->setEnabled(false);
        }
    }
    else
    {
        ui->DurationText->clear();
        ui->CurTimeText->clear();
    }
}

void FigWidget::displayTotalTime()
{
    QDateTime totalTime = travelerList[ui->TravelerComboBox->currentIndex()].totalTime;
    ui->TotalTimeEdit->setText(QString::number(totalTime.date().day()-1) + QString::fromWCharArray(L"天 ") +
                               QString::number(totalTime.time().hour()) + QString::fromWCharArray(L"小时 ") +
                               QString::number(totalTime.time().minute()) + QString::fromWCharArray(L"分钟"));
}

void FigWidget::displayFare(std::vector<TimeTable> path)
{
    double totalDanger = 0;
    for(unsigned index = 0; index != path.size(); index++)
    {
        totalDanger += path[index].danger;
    }
    qDebug() << QString::fromWCharArray(L"风险:") + QString::number(totalDanger,'f',1);
    ui->FareEdit->setText(QString::number(totalDanger));
}

QString FigWidget::num2City(int index){
    QString city;
    switch (index)
    {
    case 0:
        city = QString::fromWCharArray(L"北京"); break;
    case 1:
        city = QString::fromWCharArray(L"哈尔滨"); break;
    case 2:
        city = QString::fromWCharArray(L"长春"); break;
    case 3:
        city = QString::fromWCharArray(L"青岛");break;
    case 4:
        city = QString::fromWCharArray(L"上海"); break;
    case 5:
        city = QString::fromWCharArray(L"武汉"); break;
    case 6:
        city = QString::fromWCharArray(L"广州"); break;
    case 7:
        city = QString::fromWCharArray(L"西安"); break;
    case 8:
        city = QString::fromWCharArray(L"西宁"); break;
    case 9:
        city = QString::fromWCharArray(L"乌鲁木齐"); break;
    case 10:
        city = QString::fromWCharArray(L"拉萨"); break;
    case 11:
        city = QString::fromWCharArray(L"成都"); break;
    default:
        QMessageBox::warning(this, "Error", QString::fromWCharArray(L"程序运行错误：不存在此地"));
        break;
    }
    return city;
}

void FigWidget::displayPath(std::vector<TimeTable> plan)
{
    QVBoxLayout *listlayout = new QVBoxLayout;
    QWidget *containwidget = new QWidget(ui->PathList);

    for(unsigned index = 0; index != plan.size(); index++)
    {
        if(plan[index].num=="")
        {
            qDebug()<<"null num";
            continue;
        }
        QLabel *vehiclelabel = new QLabel;
        QLabel *textlabel = new QLabel;

        if (plan[index].vehicle == 0)
            vehiclelabel->setPixmap(QPixmap(":/res/car.png"));
        else if (plan[index].vehicle == 1)
            vehiclelabel->setPixmap(QPixmap(":/res/train.png"));
        else if (plan[index].vehicle == 2)
            vehiclelabel->setPixmap(QPixmap(":/res/plane.png"));

        textlabel->setText(" " + num2City(plan[index].from) + "->" + num2City(plan[index].to) +
                           QString::fromWCharArray(L" 班次:") + plan[index].num +
                           " " + QString::fromWCharArray(L"出发:") + plan[index].begin.toString("hh:mm") +
                           QString::fromWCharArray(L" 到站:") + plan[index].end.toString("hh:mm") +
                           QString::fromWCharArray(L" 风险:") + QString::number(plan[index].danger));

        QHBoxLayout *rowlayout = new QHBoxLayout;
        rowlayout->addStretch(1);
        rowlayout->addWidget(vehiclelabel);
        rowlayout->addWidget(textlabel);
        rowlayout->addStretch(1);
        listlayout->addLayout(rowlayout);
    }
    containwidget->setLayout(listlayout);
    ui->PathList->setWidget(containwidget);
}

FigWidget::~FigWidget()
{
    timethread->terminate();
    timethread->wait();
    delete ui;
    delete timethread;
}
