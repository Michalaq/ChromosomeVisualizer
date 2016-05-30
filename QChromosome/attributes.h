#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <QWidget>

#include "../QtChromosomeViz_v2/VizWidget.hpp"

namespace Ui {
    class Attributes;
}

class Attributes : public QWidget
{
    Q_OBJECT
public:
    explicit Attributes(QWidget *parent = 0);
    ~Attributes();

    void setVizWidget(VizWidget* vizWidget);

    void handleSelection(const AtomSelection& selection);

signals:

public slots:

private:
    Ui::Attributes *ui;

    VizWidget* vizWidget_;

    QString title;
};

#endif // ATTRIBUTES_H
