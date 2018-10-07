#ifndef GLBUFFER_H
#define GLBUFFER_H


#include <QOpenGLBuffer>

template<typename T>
class GLBuffer : public QVector<T>
{
public:
    void allocate(QOpenGLBuffer& buffer)
    {
        if (resized)
        {
            buffer.bind();
            buffer.allocate(QVector<T>::constData(), QVector<T>::count() * sizeof(T));
            buffer.release();

            resized = false;
            modified = false;
        }

        if (modified)
        {
            buffer.bind();
            buffer.write(0, QVector<T>::constData(), QVector<T>::count() * sizeof(T));
            buffer.release();

            modified = false;
        }
    }

    int emplace_back()
    {
        resized = true;
        QVector<T>::push_back(T());
        return QVector<T>::count() - 1;
    }

    void remove(int i, int count)
    {
        resized = true;
        QVector<T>::remove(i, count);
    }

    void resize(int size)
    {
        resized = true;
        QVector<T>::resize(QVector<T>::size() + size);
    }

    inline T& operator[](int i)
    {
        modified = true;
        return QVector<T>::operator[](i);
    }

    void forceReallocate()
    {
        resized = true;
    }

    void pop_back()
    {
        resized = true;
        QVector<T>::pop_back();
    }

private:
    bool modified;
    bool resized;
};

#endif // GLBUFFER_H
