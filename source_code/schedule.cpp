#include "schedule.h"


std::multimap<int, TimeTable> Schedule::trafficMap;
std::multimap<int, TimeTable> Schedule::trafficMapIvs;
std::vector<std::vector<TimeTable> > Schedule::vehicleSelect(cityAmount*cityAmount);
bool Schedule::isInited = false;

Schedule::Schedule()
{
    if(!isInited)
    {
        QFile file(":/res/test.txt");
        if(!file.open(QIODevice::ReadOnly))
        {
            qDebug() << "can't open file";
            return;
        }
        QTextStream fin(&file);
        QString from, to, num, begin, end, vehicle;
        while(!fin.atEnd())
        {
            fin >> from >> to >> num >> begin >> end >> vehicle >> endl;
            if(from != "")
            {
                int fromNum = cityToNum(from), toNum = cityToNum(to);
                TimeTable line(fromNum, toNum, num, QTime::fromString(begin), QTime::fromString(end),
                               vehicle.toInt());
                trafficMap.insert({fromNum, line});
                trafficMapIvs.insert({toNum, line});
                vehicleSelect[fromNum * cityAmount + toNum].push_back(line);
            }
        }
        isInited = true;
    }
}

int Schedule::cityToNum(QString city)
{
    int num = -1;
    if(city == "Beijing") num = 0;
    else if(city == "Harbin") num = 1;
    else if(city == "Changchun") num = 2;
    else if(city == "Qingdao") num = 3;
    else if(city == "Shanghai") num = 4;
    else if(city == "Wuhan") num = 5;
    else if(city == "Guangzhou") num = 6;
    else if(city == "Xi'an") num = 7;
    else if(city == "Xining") num = 8;
    else if(city == "Urumqi") num = 9;
    else if(city == "Lhasa") num = 10;
    else if(city == "Chengdu") num = 11;
    return num;
}
