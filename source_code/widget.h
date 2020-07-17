#ifndef WIDGET_H
#define WIDGET_H

#include "traveler.h"
#include "schedule.h"
#include "cmdwidget.h"
#include "figwidget.h"

#include <QWidget>
#include <QDebug>
#include <QIcon>
#include <QPainter>
#include <QDialog>
#include <QPushButton>
#include <QImageReader>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget {
    Q_OBJECT
public:
    explicit Widget(QWidget* parent = nullptr);	/* 创建按钮、连接信号和槽 */
    void paintEvent(QPaintEvent*);		/* 绘制背景 */
    FigWidget* figWidget = NULL;		/* 下一级页面 */
    CmdWidget* cmdWidget = NULL;		/* 下一级页面 */
    ~Widget();

private:
    std::vector<Traveler*> travelerList;	/* 旅客列表 */
    Ui::Widget* ui;							/* ui */
};
#endif // WIDGET_H
