#ifndef ATOMATTRIBUTES_H
#define ATOMATTRIBUTES_H

#include <QWidget>

#include "../QtChromosomeViz_v2/VizWidget.hpp"

namespace Ui {
    class AtomAttributes;
}

class AtomAttributes : public QWidget
{
    Q_OBJECT
public:
    explicit AtomAttributes(QWidget *parent = 0);
    ~AtomAttributes();

    void setVizWidget(VizWidget* vizWidget);

    void handleSelection(const AtomSelection& selection);

protected:
    void resizeEvent(QResizeEvent *event);

signals:

public slots:
    void updateName();
    void updateVisibility(VisibilityMode m);

private:
    Ui::AtomAttributes *ui;

    VizWidget* vizWidget_;

    QString title, list;
};

#endif // ATOMATTRIBUTES_H
