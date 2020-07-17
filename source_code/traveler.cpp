#include "traveler.h"

Traveler::Traveler(int _id, QDateTime _startTime, QDateTime _deadlineTime, int _strategy, int _source
    , int _destination) :
    id(_id), startTime(_startTime), deadlineTime(_deadlineTime), strategy(_strategy), source(_source),
    destination(_destination), usedTime(QDateTime(QDate(1, 1, 1), QTime(0, 0, 0, 0))),
    danger(cityAmount, __DBL_MAX__), dangerIvs(cityAmount, __DBL_MAX__),
    time(cityAmount, QDateTime(QDate(7999, 12, 31), QTime(23, 59, 59))), minDanger(__DBL_MAX__)
{
    time[source] = startTime;
    if (strategy == minRisk)
    {
        Simulated_Annealing();
    }
    else if (strategy == minRiskDDL)
    {
        Spfa(destination);
        plan = AStar(source, destination);
        minDanger = calDanger(plan);
    }
    else
    {
        qDebug() << "Wrong strategy num";
    }
    totalTime = TotalDateTime();
}

//模拟退火
void Traveler::Simulated_Annealing()
{
    double t = T0;// 初始温度
    while (t > Tk)
    {
        // 产生新解。随机产生下标进行交换，从而产生新序列
        std::vector<int> randomIndex(cityAmount * cityAmount, 0);
        for (unsigned i = 0; i < Schedule::vehicleSelect.size(); ++i)
        {
            int items = Schedule::vehicleSelect[i].size();
            if (items < 2) continue;
            while (!randomIndex[i]) randomIndex[i] = rand() % items;
            int j = randomIndex[i];
            Swap(Schedule::vehicleSelect[i][0], Schedule::vehicleSelect[i][j]);
        }
        // 将vehicleSelect转换为multimap<int, TimeTable> pathSelect
        std::multimap<int, TimeTable> pathSelect;
        for (int i = 0; i < cityAmount * cityAmount; ++i)
        {
            if (!Schedule::vehicleSelect[i].size())  continue;
            pathSelect.insert({ Schedule::vehicleSelect[i][0].from, Schedule::vehicleSelect[i][0] });
        }
        // 计算新解风险值
        std::vector<TimeTable> nowPlan = Dijkstra(time, pathSelect);
        double nowDanger = calDanger(nowPlan);
        double deltaDanger = nowDanger - minDanger;
        /* 接受新解更新minDanger */
        if (nowDanger < minDanger)
        {
            minDanger = nowDanger;
            plan = nowPlan;
        }
        // 以一定概率接受新解
        else if (exp(-deltaDanger / t) * RAND_MAX > rand()) {/* 不更新minDanger */}
        else // 拒绝新解
        {/* 恢复原序列 */
            for (unsigned i = 0; i < Schedule::vehicleSelect.size(); ++i)
            {
                int j = randomIndex[i];
                if (!j) continue;
                Swap(Schedule::vehicleSelect[i][0], Schedule::vehicleSelect[i][j]);
            }
        }
        t *= delta;// 降温
    }
}

//斐波那契堆优化
std::vector<TimeTable> Traveler::Dijkstra(std::vector<QDateTime>& time, std::multimap<int, TimeTable> pathSelect)
{
    FibonacciHeap  fp;
    std::vector<TimeTable> path(cityAmount);
    std::vector<double> danger(cityAmount, __DBL_MAX__);
    danger[source] = 0;
    fp.initNode(source, danger[source]);/* 初始化新结点 */
    fp.merge(source);/* 插入虚结点 */
    for (int i = 0; i < cityAmount - 1; ++i)
    {
        int u = fp.extractMin();
        if (u == -1) break;
        auto items = pathSelect.count(u);
        auto it = pathSelect.find(u);
        for (unsigned j = 0; j < items; ++j, ++it)
        {
            /* 如果v已被退火删除则直接continue */
            int v = it->second.to;
            int duration = (it->second.begin.hour() - time[u].time().hour() + 24) % 24;
            it->second.danger += cityRisk[u] * duration;
            if (danger[v] > danger[u] + it->second.danger)
            {
                if (danger[v] == __DBL_MAX__)
                {/* 初始化结点v、插入结点v */
                    danger[v] = danger[u] + it->second.danger;
                    fp.initNode(v, danger[v]);
                    fp.merge(v);
                }
                else/* 进行decreaseKey操作 */
                {
                    danger[v] = danger[u] + it->second.danger;
                    fp.decreaseDanger(v, danger[v]);
                }
                //更新path, time
                path[v] = it->second;
                updateTime(time, it, u, v);
            }
        }
    }
    /* 找终点是destination的航班tmpMinTfc */
    // 传入终点航班
    std::vector<TimeTable> plan;
    path2Plan(destination, path, plan);
    return plan;
}

void Traveler::Spfa(int source)
{
    std::vector<bool> vis(cityAmount, false);
    std::deque<int> q;
    double dangerSum = 0;
    q.push_back(source);
    dangerIvs[source] = 0;
    vis[source] = true;
    dangerSum += dangerIvs[source];
    while (!q.empty())
    {
        int now = q.front();
        //LLL优化
        while (dangerIvs[now] * q.size() > dangerSum)
        {
            q.pop_front(); q.push_back(now);
            now = q.front();
        }
        q.pop_front();
        vis[now] = false;
        dangerSum -= dangerIvs[now];
        auto items = Schedule::trafficMapIvs.count(now);
        auto it = Schedule::trafficMapIvs.find(now);
        // 对边进行松弛
        for (unsigned i = 0; i < items; ++i, ++it)
        {
            int v = it->second.from;
            if (dangerIvs[v] > dangerIvs[now] + it->second.danger)
            {
                dangerIvs[v] = dangerIvs[now] + it->second.danger;/* 松弛 */
                if (vis[v]) continue;/* 若v已经遍历，则continue */
                vis[v] = true; dangerSum += dangerIvs[v];
                //SLF优化
                if (!q.empty() && dangerIvs[v] < dangerIvs[q.front()]) q.push_front(v);
                else q.push_back(v);
            }
        }
    }
}

std::vector<TimeTable> Traveler::AStar(int source, int destination)
{
    std::vector<bool> vis(cityAmount, false);
    std::vector<TimeTable> path(cityAmount);
    std::priority_queue<node> q;
    bool canArrive = false;
    danger[source] = 0;
    q.push(node(source, 0, dangerIvs[source]));
    while (!q.empty())
    {
        node u = q.top(); q.pop();
        /* 若u已被访问，则continue */
        if (vis[u.now]) continue;
        vis[u.now] = true;
        if (u.now != destination)
        {
            auto items = Schedule::trafficMap.count(u.now);
            auto it = Schedule::trafficMap.find(u.now);
            for (unsigned i = 0; i < items; ++i, ++it)
            {
                int v = it->second.to;
                int duration = (it->second.begin.hour() - time[u.now].time().hour() + 24) % 24;
                it->second.danger += cityRisk[u.now] * duration;
                if (danger[v] > danger[u.now] + it->second.danger)
                {
                    danger[v] = danger[u.now] + it->second.danger;
                    /* 结点入队列 */
                    q.push(node(v, u.pasDanger + it->second.danger, dangerIvs[v] + u.pasDanger + it->second.danger));
                    //更新path, time
                    path[v] = it->second;
                    updateTime(time, it, u.now, v);
                }
            }
        }
        else/* 到达终点，判断时间是否符合要求 */
        {
            if (time[destination] > deadlineTime)  continue;
            canArrive = true;
            break;
        }
    }
    std::vector<TimeTable> plan;
    if (canArrive) path2Plan(destination, path, plan);
    return plan;
}

void Traveler::path2Plan(int city, const std::vector<TimeTable>& path, std::vector<TimeTable>& plan)
{
    if (path[city].from == -1) return;
    if (city != source)
    {
        path2Plan(path[city].from, path, plan);
        plan.push_back(path[city]);
    }
}

void Traveler::updateTime(std::vector<QDateTime> &time, std::multimap<int, TimeTable>::iterator it, int u, int v)
{
    bool span;
    if (it->second.begin <= it->second.end) span = false;
    else span = true;
    QDate timeDate = time[u].date();
    if (!span && time[u].time() <= it->second.begin)
        time[v] = QDateTime(timeDate, it->second.end);
    else if (!span && time[u].time() > it->second.begin)
        time[v] = QDateTime(timeDate.addDays(1), it->second.end);
    else if (span && time[u].time() <= it->second.begin)
        time[v] = QDateTime(timeDate.addDays(1), it->second.end);
    else if (span && time[u].time() > it->second.begin)
        time[v] = QDateTime(timeDate.addDays(2), it->second.end);
}

std::vector<TimeTable> Traveler::getPlan()
{
    return this->plan;
}

void Traveler::Swap(TimeTable& a, TimeTable& b)
{
    TimeTable temp = a;
    a = b;
    b = temp;
}

double Traveler::calDanger(std::vector<TimeTable> plan)
{
    double tempDanger = 0;
    for (unsigned i = 0; i < plan.size(); ++i)
    {
        tempDanger += plan[i].danger;
    }
    return tempDanger;
}

QDateTime Traveler::TotalDateTime()
{
    QDateTime endTime = time[destination];

    int durationMin = (endTime.time().minute() - startTime.time().minute());
    int durationHour = (endTime.time().hour() - startTime.time().hour() - (int)((durationMin >= 0) ? 0 : 1));
    int durationDay = (endTime.date().day() - startTime.date().day() - (int)((durationHour >= 0) ? 0 : 1)
        + startTime.date().daysInMonth()) % startTime.date().daysInMonth();
    durationMin = (durationMin + 60) % 60;
    durationHour = (durationHour + 24) % 24;

    return QDateTime(QDate(1, 1, durationDay + 1), QTime(durationHour, durationMin, 0));
}

QDateTime Traveler::getCityArvTime(int index)
{
    return time[index];
}

QDateTime Traveler::getCityDptTime(int index)
{
    unsigned id = 0;
    QTime tempTime;

    for (; id != plan.size(); id++)
        if (plan[id].from == index)
        {
            tempTime = plan[id].begin;
            break;
        }

    QDateTime DepartureDateTime = time[index];
    if (DepartureDateTime.time() <= tempTime)
        return QDateTime(DepartureDateTime.date(), tempTime);
    else
        return QDateTime(DepartureDateTime.date().addDays(1), tempTime);
}

double Traveler::getMinDanger()
{
    return this->minDanger;
}
