#ifndef LAYERATTRIBUTES_H
#define LAYERATTRIBUTES_H

#include <QWidget>
#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"
#include "../QtChromosomeViz_v2/VizWidget.hpp"

namespace Ui {
    class LayerAttributes;
}

class LayerAttributes : public QWidget
{
    Q_OBJECT
public:
    explicit LayerAttributes(QWidget *parent = 0);
    ~LayerAttributes();

    void setSimulation(std::shared_ptr<Simulation> s, VizWidget* vw);

    void handleSelection(const QList<unsigned int>& selection, const AtomSelection& as);

protected:
    void resizeEvent(QResizeEvent *event);

signals:

public slots:
    void appendLayer();

private:
    Ui::LayerAttributes *ui;

    std::shared_ptr<Simulation> simulation;
    VizWidget *vizWidget_;

    QString title, list;

    unsigned int id;
};

#endif // LAYERATTRIBUTES_H
