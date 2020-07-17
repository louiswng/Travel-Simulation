#include "changespeed.h"


ChangeSpeed::ChangeSpeed(QWidget *parent) : QWidget(parent)
{
    spin = new QSpinBox(this);
    slider = new QSlider(Qt::Horizontal, this);

    QHBoxLayout* layout = new QHBoxLayout;
    // 将控件添加到布局中
    layout->addWidget(spin);
    layout->addWidget(slider);
    // 将布局设置到窗口中
    setLayout(layout);
    spin->setValue(1);
    spin->setMinimum(1);
    spin->setMaximum(10);
    slider->setMinimum(1);
    slider->setMaximum(10);
    // 添加消息响应
    connect(spin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), slider, &QSlider::setValue);
    connect(slider, &QSlider::valueChanged, [=](){
        emit speedValChange();
    });
    connect(slider, &QSlider::valueChanged, spin, &QSpinBox::setValue);

}
