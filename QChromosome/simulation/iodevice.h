#ifndef IODEVICE_H
#define IODEVICE_H


#include <QFile>
#include <zlib.h>

class IODevice
{
public:
    virtual ~IODevice();

    virtual bool open(QIODevice::OpenMode mode) = 0;

    virtual qint64 pos() const = 0;
    virtual bool seek(qint64 pos) = 0;

    virtual bool readLine(char *data, qint64 maxSize) = 0;

    virtual void setFileName(const QString& name) = 0;
    virtual QString fileName() = 0;
};

class File : public IODevice
{
public:
    inline bool open(QIODevice::OpenMode mode) override
    {
        return file.open(mode);
    }

    inline qint64 pos() const override
    {
        return file.pos();
    }

    inline bool seek(qint64 pos) override
    {
        return file.seek(pos);
    }

    inline bool readLine(char *data, qint64 maxSize) override
    {
        std::fill(data, data + maxSize, 0);
        return file.readLine(data, maxSize) != -1;
    }

    inline void setFileName(const QString& name) override
    {
        file.setFileName(name);
    }

    inline QString fileName() override
    {
        return file.fileName();
    }

private:
    QFile file;
};

class GzFile : public IODevice
{
public:
    bool open(QIODevice::OpenMode mode) override;

    inline qint64 pos() const override
    {
        return gztell64(file);
    }

    inline bool seek(qint64 pos) override
    {
        return gzseek64(file, pos, SEEK_SET);
    }

    inline bool readLine(char *data, qint64 maxSize) override
    {
        std::fill(data, data + maxSize, 0);
        return gzgets(file, data, maxSize) != NULL;
    }

    void setFileName(const QString& name) override;
    QString fileName() override;

private:
    QString name;
    gzFile file;
};

#endif // IODEVICE_H
