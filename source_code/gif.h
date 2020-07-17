#ifndef GIF_H
#define GIF_H

#include "widget.h"

#include <QWidget>
#include <QTextEdit>
#include <QBrush>
#include <QPalette>
#include <QPixmap>
#include <QPen>
#include <QDebug>
#include <QRectF>
#include <QDateTime>
#include <QPointF>
#include <QMessageBox>
#include <QTimer>
#include <QThread>
#include<QDebug>

class Gif : public QWidget
{
public:
    explicit Gif(QWidget* parent = nullptr); /* 绘制背景、建立时间线程、连接信号和槽 */
private:
    void update();					/* 刷新画面 */
    void paintEvent(QPaintEvent*);	/* 绘图 */
    QPixmap setPointGraph();		/* 设置图标 */
    QPointF setPointPos();			/* 设置图标位置 */
    QDateTime getSplitTime(QDateTime, QDateTime);	/* 获取两时间点时间间隔 */
    QPointF getCityCor(int city);					/* 获得城市对应坐标 */
    double getTimeDifference(QDateTime, QDateTime);	/* 获得两时间间隔时间差 */
    QPointF getMoveDistance(QDateTime, QDateTime, QDateTime, int, int);/* 获得坐标增量 */
    int state;				/* 旅客状态 */
    QTimer* paintmstimer;	/* 刷新计时器 */
};

#endif // GIF_H
