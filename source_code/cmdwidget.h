#ifndef CMDWIDGET_H
#define CMDWIDGET_H

#include "traveler.h"
#include "schedule.h"
#include "mypushbutton.h"

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMessageBox>
#include <QPixmap>
#include <QTimer>

namespace Ui {
class CmdWidget;
}

class CmdWidget : public QWidget {
    Q_OBJECT
public:
    explicit CmdWidget(QWidget* parent = nullptr);	/* 创建按钮、连接信号和槽 */
    void paintEvent(QPaintEvent*);				/* 绘制背景 */
    std::vector<QString> handleInput(QString);	/* 处理输入信息 */
    QString num2City(int);	/* 序号转换为城市字符 */
    ~CmdWidget();			/* 析构函数 */

    int addNum;			/* 添加乘客次数 */
    bool allowInput;		/* 是否可以输入 */
    std::vector<Traveler> travelerList;	/* 旅客列表 */
    Schedule schedule;					/* 读取文件 */
signals:
    void cmdWidgetBack();	/* 命令行界面返回信号 */
    void handleInfo();	/* 处理信息信号 */
private:
    Ui::CmdWidget* ui;		/* ui */
};

#endif // CMDWIDGET_H
