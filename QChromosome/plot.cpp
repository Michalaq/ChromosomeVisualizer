#include "plot.h"

// @bartekz: to będzie można usunąć jak zaczniesz używać drugiego konstruktora
#include "../QtChromosomeViz_v2/bartekm_code/PDBSimulation.h"
static const char * SIMULATION_PATH = "/home/bart/Pobrane/MC_random_r10_avoid_last_b700.pdb";
        //"D:\\kodziki\\bio\\MC_random_r10_avoid_last_b700.pdb";
        //"/home/bartek/Dokumenty/zpp/test.pdb";

Plot::Plot(QWidget *parent) : QWidget(parent), simulation_(std::make_shared<PDBSimulation>(SIMULATION_PATH))//simulation_(nullptr)
{

}

Plot::~Plot()
{

}

void Plot::setSimulation(std::shared_ptr<Simulation> dp)
{
    simulation_ = std::move(dp);
    update();
}

#include <QPainter>

void Plot::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (!simulation_)
        return;

    QPainter painter(this);
    painter.setWindow(0, height(), width(), -height());

    qreal x = 0, y;
    qreal dx = (qreal)width() / 99;

    QPainterPath path;
    path.moveTo(0, 0);

    for (int i = 0; i < 100; i++, x += dx)
    {
        y = simulation_->getFrame(i)->functionValues["bonds"];
        path.lineTo(x, y);
    }

    path.lineTo(x, 0);
    path.closeSubpath();

    painter.fillPath(path, Qt::red);
}
