#ifndef CANVAS_H
#define CANVAS_H

#include "draggable.h"
#include "camera.h"
#include "../QtChromosomeViz_v3/VizWidget.hpp"


/* viewport widget */
class Canvas : public Draggable
{
    Q_OBJECT

public:
    Canvas(QWidget *parent = 0);
    virtual ~Canvas();

    void setScene(VizWidget *newscene);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void wheelEvent(QWheelEvent *event);

private:
    Camera *editorCamera;

    VizWidget *scene;

    void updateScene();

    const qreal distanceFactor = 0.02;
    const qreal angleFactor = 0.05;
    const qreal wheelFactor = 2.00;

    Qt::Key modifier;

public slots:
    void move(int dx, int dy);
    void rotate(int dx, int dy);
    void scale(int dx, int dy);
};

#endif // CANVAS_H
