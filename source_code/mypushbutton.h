#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QPushButton>
#include <QDebug>
#include <QPropertyAnimation>

class MyPushButton : public QPushButton {
    Q_OBJECT
public:
    MyPushButton(QString normalImg, QString pressImg = "");	/* 加载图片 */
    void zoom1(); /* 向下跳 */
    void zoom2(); /* 向上跳 */

private:
    QString normalImgPath;  /* 默认显示图片路径 */
    QString pressedImgPath; /* 按下后显示图片路径 */
};

#endif // MYPUSHBUTTON_H
