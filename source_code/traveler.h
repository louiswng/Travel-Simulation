#ifndef TRAVELER_H
#define TRAVELER_H

#include "schedule.h"
#include "fibonacciheap.h"

#include <vector>
#include <cmath>
#include <queue>
#include <QDebug>

enum Strategy{minRisk = 1, minRiskDDL = 2};//枚举旅行策略变量值

const double delta = 0.9;
const double T0 = 100;
const double Tk = 1e-4;

struct node
{
    int now;//边起点
    double pasDanger,pasGuess;//到达now的边权、估值
    node(int _now, double _pasDanger, double _pasGuess) : now(_now), pasDanger(_pasDanger),
        pasGuess(_pasGuess) {}
    bool operator<(const node& a) const { return pasGuess > a.pasGuess; }
};

class Traveler {		/* 乘客信息及规划路径 */
public:
    Traveler(int, QDateTime, QDateTime, int, int, int); /* 构造函数赋初值 */
    std::vector<TimeTable> getPlan();	/* 获取旅客计划 */
    QDateTime getCityArvTime(int);		/* 获取城市到达时间 */
    QDateTime getCityDptTime(int);		/* 获取城市离开时间 */
    double getMinDanger();				/* 获取最小风险值 */

    int id, strategy, source, destination;		/* 分别记录旅客序号、策略、起点城市序号、终点城市序号 */
    QDateTime startTime, deadlineTime, usedTime, totalTime;	/* 分别记录旅客起始时间、限制时间、已用时间、计划总时间 */
private:
    double calDanger(std::vector<TimeTable>);	/* 计算规划路线的总风险值 */
    void Simulated_Annealing();					/* 模拟退火算法 */
    std::vector<TimeTable> Dijkstra(std::vector<QDateTime>&, std::multimap<int, TimeTable>);									/* 斐波那契堆优化的Dijkstra算法 */
    void Spfa(int);								/* SLF+LLL优化的SPFA算法 */
    std::vector<TimeTable> AStar(int, int);		/* 求解k短路问题的A*算法 */
    void path2Plan(int, const std::vector<TimeTable>&, std::vector<TimeTable>&);	/* 逆向求解计划 */
    void Swap(TimeTable&, TimeTable&);		/* 交换航班信息（模拟退火中使用） */
    QDateTime TotalDateTime();					/* 计算totalTime */
    void updateTime(std::vector<QDateTime>&, std::multimap<int, TimeTable>::iterator, int, int);				/* 更新time */

    std::vector<double> danger;		/* 从起点到达结点的风险值 */
    std::vector<double> dangerIvs;	/* 航班到destination的风险值 */
    std::vector<TimeTable> plan;	/* 规划的路线 */
    std::vector<QDateTime> time;	/* 到达每个结点的时间 */
    double minDanger;				/* 最小风险值 */
};

#endif // TRAVELER_H
