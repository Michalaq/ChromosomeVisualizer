#include "viewport.h"
#include "ui_viewport.h"

Viewport::Viewport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewport)
{
    ui->setupUi(this);

    ui->widget->setValue(QColor(Qt::black));
    ui->widget_2->setValue(QColor(Qt::black));
    ui->widget_3->setValue(QColor(Qt::black));
    ui->widget_4->setValue(QColor(Qt::white));
}

Viewport::~Viewport()
{
    delete ui;
}
