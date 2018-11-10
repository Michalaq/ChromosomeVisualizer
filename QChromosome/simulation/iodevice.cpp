#include "iodevice.h"

IODevice::~IODevice()
{

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

void GzFile::setFileName(const QString& name)
{
    this->name = name;
}

QString GzFile::fileName()
{
    return name;
}
