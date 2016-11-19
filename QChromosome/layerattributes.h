#ifndef LAYERATTRIBUTES_H
#define LAYERATTRIBUTES_H

#include <QWidget>
#include "../QtChromosomeViz_v2/VizWidget.hpp"
#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"

namespace Ui {
    class LayerAttributes;
}

class LayerAttributes : public QWidget
{
    Q_OBJECT
public:
    explicit LayerAttributes(QWidget *parent = 0);
    ~LayerAttributes();

    void setVizWidget(VizWidget* vizWidget);
    void setSimulation(std::shared_ptr<Simulation> s);

    void handleSelection(const AtomSelection& selection, const QList<unsigned int>& layers);

protected:
    void resizeEvent(QResizeEvent *event);

signals:

public slots:
    void appendLayer();

private:
    Ui::LayerAttributes *ui;

    VizWidget *vizWidget_;
    std::shared_ptr<Simulation> simulation;

    QString title, list;

    unsigned int id;
};

#endif // LAYERATTRIBUTES_H
