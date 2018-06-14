#ifndef SESSION_H
#define SESSION_H


#include <QOpenGLBuffer>

template<typename T>
class GLBuffer
{
public:
    GLBuffer()
    {

    }

    void allocate(QOpenGLBuffer& buffer)
    {
        if (resized)
        {
            buffer.bind();
            buffer.allocate(data.constData(), data.count() * sizeof(T));
            buffer.release();

            resized = false;
            modified = false;
        }

        if (modified)
        {
            buffer.bind();
            buffer.write(0, data.constData(), data.count() * sizeof(T));
            buffer.release();

            modified = false;
        }
    }

    const QVector<T>& get() const
    {
        return data;
    }

    void resize(size_t s)
    {
        resized = true;
        data.resize(data.size() + s);
    }

    const T& operator[](int n) const
    {
        return data[n];
    }

    T& operator[](int n)
    {
        modified = true;
        return data[n];
    }

private:
    bool modified;
    bool resized;

    QVector<T> data;
};

#include <QAction>
#include "treeitem.h"
#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"

template class GLBuffer<VizCameraInstance>;
template class GLBuffer<VizBallInstance>;

class Session
{
public:
    Session();
    ~Session();

    QAction *action;
    Simulation *simulation;

    GLBuffer<VizCameraInstance> cameraBuffer;
    GLBuffer<VizBallInstance> atomBuffer;

    void setFrame(std::shared_ptr<Frame> frame);
};

#endif // SESSION_H
