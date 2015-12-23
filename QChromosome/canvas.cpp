#include "canvas.h"

Canvas::Canvas(QWidget *parent) : Draggable(parent), editorCamera(new Camera(this)), scene(nullptr), modifier(Qt::Key_unknown)
{
    setFocus();
}

Canvas::~Canvas()
{

}

void Canvas::updateScene()
{
    if (scene)
    {
        scene->setModelView(editorCamera->getModelView());
        scene->update();
    }
}

void Canvas::setScene(VizWidget *newscene)
{
    scene = newscene;
    scene->setParent(this);

    updateScene();
}

void Canvas::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return event->ignore();

    if (modifier == Qt::Key_unknown)
    {
        switch (event->key())
        {
        case Qt::Key_M:
            modifier = Qt::Key_M;
            connect(this, SIGNAL(delta(int,int)), this, SLOT(move(int,int)));
            break;

        case Qt::Key_R:
            modifier = Qt::Key_R;
            connect(this, SIGNAL(delta(int,int)), this, SLOT(rotate(int,int)));
            break;

        case Qt::Key_S:
            modifier = Qt::Key_S;
            connect(this, SIGNAL(delta(int,int)), this, SLOT(scale(int,int)));
            break;
        }
    }

    Draggable::keyPressEvent(event);
}

void Canvas::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return event->ignore();

    if (modifier == event->key())
    {
        modifier = Qt::Key_unknown;
        disconnect();
    }

    Draggable::keyReleaseEvent(event);
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    Draggable::wheelEvent(event);

    scale(wheelFactor * event->angleDelta().y(), 0.);
}

void Canvas::move(int dx, int dy)
{
    editorCamera->move(distanceFactor * dx, -distanceFactor * dy, 0.);
    updateScene();
}

void Canvas::rotate(int dx, int dy)
{
    editorCamera->rotate(angleFactor * dx, angleFactor * dy, 0.);
    updateScene();
}

void Canvas::scale(int dx, int dy)
{
    Q_UNUSED(dy)

    editorCamera->move(0., 0., distanceFactor * dx);
    updateScene();
}
