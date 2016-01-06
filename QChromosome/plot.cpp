#include "plot.h"

// @bartekz: to będzie można usunąć jak zaczniesz używać drugiego konstruktora
#include "../QtChromosomeViz_v2/bartekm_code/PDBSimulation.h"
static const char * SIMULATION_PATH = "/home/bart/Pobrane/MC_random_r10_avoid_last_b700.pdb";
        //"D:\\kodziki\\bio\\MC_random_r10_avoid_last_b700.pdb";
        //"/home/bartek/Dokumenty/zpp/test.pdb";

Plot::Plot(QWidget *parent) : QWidget(parent), simulation_(nullptr)
{
    setSimulation(std::make_shared<PDBSimulation>(SIMULATION_PATH)); //TODO do wywalenia
}

Plot::~Plot()
{

}

void Plot::setSimulation(std::shared_ptr<Simulation> dp)
{
    simulation_ = std::move(dp);

    frameNumber_t n = simulation_->getFrameCount()+100;

    data.moveTo(0, simulation_->getFrame(0)->functionValues["bonds"]);

    for (frameNumber_t i = 1; i < n; i++)
    {
        qreal y = simulation_->getFrame(i)->functionValues["bonds"];
        data.lineTo(i, y);
    }

    QRectF rect = data.boundingRect();

    size = QSize(rect.width(), rect.y() + rect.height());

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
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path(data);
    path.lineTo(data.currentPosition().x(), 0);
    path.lineTo(0, 0);
    path.closeSubpath();

    painter.scale((qreal) width() / size.width(), (qreal) height() / size.height());

    painter.fillPath(path, Qt::blue);
}
