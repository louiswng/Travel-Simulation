#include "gif.h"

Gif::Gif(QWidget *parent) : QWidget(parent)
{
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/res/map.png")));
    this->setPalette(palette);

    paintmstimer = new QTimer;
    paintmstimer->start(1000/60);
    QObject::connect(paintmstimer, SIGNAL(timeout()), this, SLOT(update()));
}

//绘图
void Gif::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    FigWidget *fatherPtr = (FigWidget *)parentWidget();
    if (fatherPtr->currentTraveler != -1)
    {
        painter.drawPixmap((setPointPos()), setPointGraph());
    }
}

QPixmap Gif::setPointGraph()
{
    QPixmap pointGraph;
    switch(state)
    {
    case -2:
        pointGraph = QPixmap(":/res/arrive.png"); break;
    case -1:
        pointGraph = QPixmap(":/res/wait.png"); break;
    case 0:
        pointGraph = QPixmap(":/res/car.png"); break;
    case 1:
        pointGraph = QPixmap(":/res/train.png"); break;
    case 2:
        pointGraph = QPixmap(":/res/plane.png"); break;
    default:
        qDebug()<< "wrong state"; break;
    }
    return pointGraph;
}

QPointF Gif::setPointPos()
{
    FigWidget *fatherPtr = (FigWidget *)parentWidget();
    static QPointF pointPos;
    std::vector<TimeTable> path = fatherPtr->travelerList[fatherPtr->currentTraveler].getPlan();
    QDateTime spenttime = fatherPtr->travelerList[fatherPtr->currentTraveler].usedTime;
    QDateTime starttime = fatherPtr->travelerList[fatherPtr->currentTraveler].startTime;

    if(spenttime >= fatherPtr->travelerList[fatherPtr->currentTraveler].totalTime)
    {
         if(spenttime == fatherPtr->travelerList[fatherPtr->currentTraveler].totalTime)
         {
              qDebug() << "Arriving at destination" << path[path.size()-1].to;
         }
         pointPos = getCityCor(path[path.size()-1].to);
         state = -2;
    }
    else
        for (unsigned index = 0; index != path.size(); index++)
        {
            QDateTime departuredatetime = fatherPtr->travelerList[fatherPtr->currentTraveler].getCityDptTime
                    (path[index].from);
            QDateTime cityarrivaltime = fatherPtr->travelerList[fatherPtr->currentTraveler].getCityArvTime
                    (path[index].to);
            QDateTime start2Begin = getSplitTime(starttime, departuredatetime);
            QDateTime start2End = getSplitTime(starttime, cityarrivaltime);

            if (spenttime <= start2Begin)
            {
                pointPos = getCityCor(path[index].from);
                state = -1;
                break;
            }
            else if (spenttime <= start2End)
            {
                pointPos = getCityCor(path[index].from);
                pointPos += getMoveDistance(spenttime, start2Begin, start2End, path[index].from, path[index].to);
                state = path[index].vehicle;
                if (spenttime == start2End)
                {
                    qDebug() << "Arriving at " << path[index].to;
                }
                break;
            }
        }
    return pointPos;
}

QDateTime Gif::getSplitTime(QDateTime former, QDateTime later)
{
    int durationSec = (later.time().second() - former.time().second());
    int durationMin = (later.time().minute() - former.time().minute() - (int)((durationSec >= 0)?0:1));
    int durationHour = (later.time().hour() - former.time().hour() - (int)((durationMin >= 0)?0:1));
    int durationDay = (later.date().day() - former.date().day() - (int)((durationHour >= 0)?0:1) +
                       former.date().daysInMonth()) % former.date().daysInMonth();
    durationSec = (durationSec + 60) % 60;
    durationMin = (durationMin + 60) % 60;
    durationHour = (durationHour + 24) % 24;

    return QDateTime(QDate(1, 1, durationDay+1), QTime(durationHour, durationMin, durationSec, 999));
}

QPointF Gif::getCityCor(int city)
{
    int x, y;
    switch (city)
    {
    case 0:
        x = 565-20; y = 249-20;
        break;
    case 1:
        x = 718-20; y = 148-20;
        break;
    case 2:
        x = 699-20; y = 196-20;
        break;
    case 3:
        x = 634-20; y = 296-20;
        break;
    case 4:
        x = 634-20; y = 394-20;
        break;
    case 5:
        x = 530-20; y = 404-20;
        break;
    case 6:
        x = 521-20; y = 515-20;
        break;
    case 7:
        x = 450-20; y = 358-20;
        break;
    case 8:
        x = 342-20; y = 309-20;
        break;
    case 9:
        x = 138-20; y = 189-20;
        break;
    case 10:
        x = 202-20;  y = 423-20;
        break;
    case 11:
        x = 380-20; y = 419-20;
        break;
    }

    return QPointF(x, y);
}

double Gif::getTimeDifference(QDateTime former, QDateTime later)
{

    int durationSec = (later.time().second() - former.time().second());
    int durationMin = (later.time().minute() - former.time().minute() - (int)((durationSec >= 0)?0:1));
    int durationHour = (later.time().hour() - former.time().hour() - (int)((durationMin >= 0)?0:1));
    int durationDay = (later.date().day() - former.date().day() - (int)((durationHour >= 0)?0:1) +
                       former.date().daysInMonth()) % former.date().daysInMonth();
    durationSec = (durationSec + 60) % 60;
    durationMin = (durationMin + 60) % 60;
    durationHour = (durationHour + 24) % 24;

    return (double)(durationDay * 86400 + durationHour * 3600 + durationMin * 60 + durationSec);
}

QPointF Gif::getMoveDistance(QDateTime spentTime, QDateTime start2Begin, QDateTime start2End,
                                   int from, int to)
{
    double increaseRatio = getTimeDifference(start2Begin, spentTime)/getTimeDifference(start2Begin, start2End);
    double xIncrease = (getCityCor(to) - getCityCor(from)).x() * increaseRatio;
    double yIncrease = (getCityCor(to) - getCityCor(from)).y() * increaseRatio;

    return QPointF(xIncrease, yIncrease);
}

