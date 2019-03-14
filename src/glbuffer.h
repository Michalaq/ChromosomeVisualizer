#ifndef GLBUFFER_H
#define GLBUFFER_H


#include <QVector>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_1_Core>

template<typename T>
class OpenGLArraysBuffer
{
public:
    void allocate(QOpenGLBuffer& buffer)
    {
        if (resized)
        {
            buffer.bind();
            buffer.allocate(this->buffer.constData(), this->buffer.count() * sizeof(T));
            buffer.release();

            resized = false;
            modified = false;
        }

        if (modified)
        {
            buffer.bind();
            buffer.write(0, this->buffer.constData(), this->buffer.count() * sizeof(T));
            buffer.release();

            modified = false;
        }
    }

    int append(const T& value)
    {
        if (firsts.isEmpty() || firsts[0] != 0)
        {
            firsts.prepend(0);
            counts.prepend(0);
        }

        int i = counts[0]++;

        if (firsts.count() == 1)
        {
            buffer.append(value);

            resized = true;
            return i;
        }

        buffer.replace(i, value);

        if (counts[0] == firsts[1])
        {
            counts[0] += counts[1];

            firsts.removeAt(1);
            counts.removeAt(1);
        }

        modified = true;
        return i;
    }

    inline T* data()
    {
        modified = true;
        return buffer.data();
    }

    void remove(int i, int count)
    {
        int j = firsts.count() - 1;

        while (j >= 0 && firsts[j] > i)
            j--;

        Q_ASSERT(i >= firsts[j] && i + count <= firsts[j] + counts[j]);

        if (i == firsts[j])
        {
            if (count == counts[j])
            {
                firsts.removeAt(j);
                counts.removeAt(j);
            }
            else
            {
                firsts[j] += count;
                counts[j] -= count;
            }
        }
        else
        {
            if (i + count == firsts[j] + counts[j])
            {
                counts[j] -= count;
            }
            else
            {
                firsts.insert(j, firsts[j]);
                firsts[j + 1] = i + count;

                counts.insert(j, i - firsts[j]);
                counts[j + 1] = firsts[j] + counts[j + 1] - firsts[j + 1];
            }
        }

        if (firsts.isEmpty())
            buffer.clear();
        else
            buffer.resize(firsts.last() + counts.last());
    }

    inline int size() const
    {
        return buffer.size();
    }

    inline T& operator[](int i)
    {
        modified = true;
        return buffer[i];
    }

    inline const T& operator[](int i) const
    {
        return buffer[i];
    }

    inline void glDrawArrays(QOpenGLFunctions_4_1_Core* object) const
    {
        object->glMultiDrawArrays(GL_POINTS, firsts.constData(), counts.constData(), firsts.size());
    }

    void forceReallocate()
    {
        resized = true;
    }

private:
    bool modified;
    bool resized;

    QVector<T> buffer;
    QVector<GLint> firsts;
    QVector<GLsizei> counts;

    friend class Simulation;
};

template<typename T>
class OpenGLElementsBuffer
{
public:
    void allocate(QOpenGLBuffer& buffer)
    {
        if (resized)
        {
            buffer.bind();
            buffer.allocate(this->buffer.constData(), this->buffer.count() * sizeof(T));
            buffer.release();

            resized = false;
            modified = false;
        }

        if (modified)
        {
            buffer.bind();
            buffer.write(0, this->buffer.constData(), this->buffer.count() * sizeof(T));
            buffer.release();

            modified = false;
        }
    }

    GLvoid* append(GLsizei count)
    {
        GLintptr offset = 0;
        int i = 0;

        while (i < firsts.size() && offset + count * sizeof(T) >= reinterpret_cast<GLintptr>(firsts[i]))
        {
            offset = reinterpret_cast<GLintptr>(firsts[i]) + counts[i] * sizeof(T);
            i++;
        }

        if (i == firsts.size())
        {
            buffer.resize(buffer.size() + count);
            resized = true;
        }

        firsts.insert(i, reinterpret_cast<GLvoid*>(offset));
        counts.insert(i, count);

        return reinterpret_cast<GLvoid*>(offset);
    }

    inline T* data()
    {
        modified = true;
        return buffer.data();
    }

    void remove(GLvoid* offset)
    {
        auto i = firsts.indexOf(offset);

        firsts.removeAt(i);
        counts.removeAt(i);

        if (firsts.isEmpty())
            buffer.clear();
        else
            buffer.resize(reinterpret_cast<GLintptr>(firsts.last()) / sizeof(T) + counts.last());
    }

    inline void glDrawElements(QOpenGLFunctions_4_1_Core* object) const
    {
        object->glMultiDrawElements(GL_LINE_STRIP, counts.data(), GL_UNSIGNED_INT, firsts.data(), counts.size());
    }

    void forceReallocate()
    {
        resized = true;
    }

private:
    bool modified;
    bool resized;

    QVector<T> buffer;
    QVector<GLvoid*> firsts;
    QVector<GLsizei> counts;

    friend class Simulation;
};

#endif // GLBUFFER_H
