#ifndef IODEVICE_H
#define IODEVICE_H


#include <QFile>
#include <zlib.h>

class IODevice
{
public:
    virtual ~IODevice();

    virtual bool open(QIODevice::OpenMode mode) = 0;

    inline virtual qint64 pos() const = 0;
    inline virtual bool seek(qint64 pos) = 0;

    inline virtual bool readLine(char *data, qint64 maxSize) = 0;

    virtual void setFileName(const QString& name) = 0;
    virtual QString fileName() = 0;
};

class File : public IODevice
{
public:
    bool open(QIODevice::OpenMode mode) override;

    inline qint64 pos() const override;
    inline bool seek(qint64 pos) override;

    inline bool readLine(char *data, qint64 maxSize) override;

    void setFileName(const QString& name) override;
    QString fileName() override;

private:
    QFile file;
};

class GzFile : public IODevice
{
public:
    bool open(QIODevice::OpenMode mode) override;

    inline qint64 pos() const override;
    inline bool seek(qint64 pos) override;

    inline bool readLine(char *data, qint64 maxSize) override;

    void setFileName(const QString& name) override;
    QString fileName() override;

private:
    QString name;
    gzFile file;
};

#endif // IODEVICE_H
