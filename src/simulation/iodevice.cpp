#include "iodevice.h"

IODevice::~IODevice()
{

}

bool GzFile::open(QIODevice::OpenMode mode)
{
    if (mode.testFlag(QIODevice::ReadOnly))
    {
        file = gzopen(name.toLocal8Bit(), "rb");
        return file != nullptr;
    }

    if (mode.testFlag(QIODevice::WriteOnly))
    {
        file = gzopen(name.toLocal8Bit(), "wb");
        return file != nullptr;
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
