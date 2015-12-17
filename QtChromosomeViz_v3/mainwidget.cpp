#include "mainwidget.h"
#include "ui_form.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent), ui(new Ui::Form)
{
    ui->setupUi(this);

    ui->widget_3->load(QString("../QtChromosomeViz/move_icon.svg"));
    ui->widget_2->load(QString("../QtChromosomeViz/rotate_icon.svg"));
    ui->widget->load(QString("../QtChromosomeViz/scale_icon.svg"));

    QPalette p(palette());
    p.setColor(QPalette::Background, QColor::fromCmykF(0.0, 0.0, 0.0, 0.9));
    setPalette(p);

    p.setColor(QPalette::Background, QColor::fromCmykF(0.0, 0.0, 0.0, 0.7));
    ui->widget_4->setAutoFillBackground(true);
    ui->widget_4->setPalette(p);
}

MainWidget::~MainWidget()
{
    delete ui;
}

