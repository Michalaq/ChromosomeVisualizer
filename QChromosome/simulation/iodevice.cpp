#include "iodevice.h"

IODevice::~IODevice()
{

}

bool File::open(QIODevice::OpenMode mode)
{
    return file.open(mode);
}

inline qint64 File::pos() const
{
    return file.pos();
}

inline bool File::seek(qint64 pos)
{
    return file.seek(pos);
}

inline bool File::readLine(char *data, qint64 maxSize)
{
    return file.readLine(data, maxSize) != -1;
}

void File::setFileName(const QString& name)
{
    file.setFileName(name);
}

QString File::fileName()
{
    return file.fileName();
}

bool GzFile::open(QIODevice::OpenMode mode)
{
    if (mode.testFlag(QIODevice::ReadOnly))
    {
        file = gzopen64(name.toLocal8Bit(), "rb");
        return true;
    }

    if (mode.testFlag(QIODevice::WriteOnly))
    {
        file = gzopen64(name.toLocal8Bit(), "wb");
        return true;
    }

    return false;
}

inline qint64 GzFile::pos() const
{
    return gztell64(file);
}

inline bool GzFile::seek(qint64 pos)
{
    return gzseek64(file, pos, SEEK_SET) != -1;
}

inline bool GzFile::readLine(char *data, qint64 maxSize)
{
    return gzgets(file, data, maxSize) != NULL;
}

void GzFile::setFileName(const QString& name)
{
    this->name = name;
}

QString GzFile::fileName()
{
    return name;
}
