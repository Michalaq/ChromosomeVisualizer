#include "simulationitem.h"
#include "messagehandler.h"
#include "treeitem.h"
#include <QFileInfo>

SimulationItem::SimulationItem(const QString& name, Session* s, int f, int l, int t) :
    session(s),
    first(f),
    last(l),
    stride(t)
{
    QFileInfo info(name);

    if (info.suffix() == "gz")
        file = new GzFile;
    else
        file = new File;

    file->setFileName(name);

    if (!file->open(QIODevice::ReadOnly))
        throw MessageLog({QtCriticalMsg, "File could not be opened.", name, nullptr, -1, -1});
}

SimulationItem::~SimulationItem()
{
    delete file;
}

void SimulationItem::write(QJsonObject& json) const
{
    json["File name"] = file->fileName();
    json["First"] = first;
    json["Last"] = last;
    json["Stride"] = stride;
}
