#ifndef FIGWIDGET_H
#define FIGWIDGET_H

#include "schedule.h"
#include "traveler.h"
#include"changespeed.h"

#include <QWidget>
#include<QTimer>
#include <QThread>
#include <QMessageBox>
#include <QPixmap>

namespace Ui {
class FigWidget;
}

class FigWidget : public QWidget {
    Q_OBJECT
signals:
    void DoStartTimer();	/* 开始计时的信号 */

public:
    explicit FigWidget(QWidget* parent = nullptr);	/* 初始化ui、建立时间线程及连接信号和槽 */
    void initFigureUI();		/* 初始化ui界面 */
    void initFigureConnect();	/* 连接信号和槽 */
    void initTimeThread();		/* 建立时间线程 */

    QDateTime getStartTime();	/* 获取开始时间 */
    QDateTime getSpentTime();	/* 获取已用时间 */
    QDateTime getDeadline();	/* 获取截止时间 */
    int getStrategy();			/* 获取用户所选策略 */
    int getStart();				/* 获取用户所选始发地 */
    int getDestination();		/* 获取用户所选目的地 */
    void showPlan(int);			/* 传入id，打印旅客的计划 */

    void displayTotalTime();		/* 显示方案所需总时间 */
    void displayFare(std::vector<TimeTable> path);/* 显示方案所需经费 */
    void displayPath(std::vector<TimeTable> path);/* 在pathlist窗口中显示路径 */
    void displayCurTime();			/* 显示旅客当前时间 */
    QString num2City(int index);	/* 将城市编号转为城市名称 */
    void startButtonClicked();		/* 开始按钮按下，开始计算路径图形输出 */
    void addTravelerButtonClicked();/* 添加乘客，按钮初始化 */
    void timeStart();				/* 计时信号 */
    void displaySpentTime();		/* 显示已经花费的时间 */

    ~FigWidget();

    Schedule schedule;					/* 读取文件 */
    int currentTraveler;				/* 当前旅客序号 */
    std::vector<Traveler> travelerList; /* 旅客列表 */

signals:
    void figWidgetBack();		/* 页面返回信号 */

public slots:
    void travelerChanged();	/* 切换旅客时更改界面显示 */

private:
    Ui::FigWidget* ui;	/* ui */

    int strategy, start, destination;	/* 策略、出发地、目的地 */
    int addtravelertimes;				/* 添加旅客次数，即旅客最大编号 */
    int startclickedtimes;				/* “开始”按钮点击次数 */
    int strategyBtnIdx;					/* 策略按钮索引 */
    std::vector<bool> startclicked;		/* “开始”按钮第一次按下 */

    //参与时间进程的变量
    QTimer* mstimer;		/* 计时器 */
    QThread* timethread;	/* 计时器线程 */
};

#endif // FIGWIDGET_H
