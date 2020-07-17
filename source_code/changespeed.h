#ifndef CHANGESPEED_H
#define CHANGESPEED_H

#include <QWidget>
#include <QSpinBox>
#include <QSlider>
#include <QHBoxLayout>
#include <QDebug>

class ChangeSpeed : public QWidget {
    Q_OBJECT
public:
    explicit ChangeSpeed(QWidget* parent = nullptr);	/* 连接slider和spin */
    QSpinBox* spin;		/* 建立spin控件 */
    QSlider* slider;	/* 建立slider控件 */
signals:
    void speedValChange();	/* 速度值改变信号 */
};

#endif // CHANGESPEED_H
