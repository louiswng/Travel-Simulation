#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <QString>
#include <map>
#include <QTime>
#include <vector>
#include <QFile>
#include <QDebug>
#include <QTextStream>

static const int cityAmount = 12;//	城市总数量
enum Vehicle { car = 1, train = 2, flight = 3 };//	枚举交通工具变量值
static std::vector<double> cityRisk = { 0.9, 0.9, 0.2, 0.9, 0.5 ,0.2, 0.5, 0.2, 0.2, 0.2, 0.2, 0.5, 0.9, 0.9, 0.5 };// 城市风险值
static std::vector<double> trafficRisk = { 2.0, 5.0, 9.0 };// 交通工具风险值

struct TimeTable {// 每个航班信息的结构体
    int from, to, vehicle;		/* 记录交通起点、终点、类型 */
    double danger;				/* 乘坐交通工具的风险 */
    QString num;				/* 交通编号 */
    QTime begin, end;			/* 交通起始、结束时间 */
    TimeTable() { from = -1; }		/* 默认构造函数 */
    TimeTable(int _from, int _to, QString _num, QTime _begin, QTime _end, int _vehicle) :
        from(_from), to(_to), num(_num), begin(_begin), end(_end), vehicle(_vehicle) {// 初始化列表
        // 计算乘坐交通工具的风险
        double duration = (_end.hour() + 24 - _begin.hour()) % 24;
        danger = cityRisk[_from] * trafficRisk[_vehicle] * duration;
    }
};

class Schedule {
public:
    Schedule();						/* 读取文件并将数据存入trafficMap, trafficMapIvs, vehicleSelect */
    static int cityToNum(QString);	/* 城市字符转换为编号 */

    static std::multimap<int, TimeTable> trafficMap;// 存航班表
    static std::multimap<int, TimeTable> trafficMapIvs;// 存逆航班表
    static std::vector<std::vector<TimeTable> > vehicleSelect;// 存每条路可能的航班

private:
    static bool isInited;	/* 保证仅读取一次文件 */
};

#endif // SCHEDULE_H
