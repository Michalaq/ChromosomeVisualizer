#ifndef LAYERATTRIBUTES_H
#define LAYERATTRIBUTES_H

#include <QWidget>
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

    void setSimulation(std::shared_ptr<Simulation> s);

    void handleSelection(const QList<unsigned int>& selection);

protected:
    void resizeEvent(QResizeEvent *event);

signals:

public slots:
    void appendLayer();

private:
    Ui::LayerAttributes *ui;

    std::shared_ptr<Simulation> simulation;

    QString title, list;

    unsigned int id;
};

#endif // LAYERATTRIBUTES_H
