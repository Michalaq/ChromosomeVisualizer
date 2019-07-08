#include "simulationitem.h"
#include "messagehandler.h"
#include "treeitem.h"
#include <QFileDialog>

SimulationItem::SimulationItem(const QString& name, Session* s, int f, int l, int t) :
    session(s),
    first(f),
    last(l),
    stride(t),
    model(nullptr)
{
    QFileInfo info(name);

    if (info.suffix() == "gz")
        file = new GzFile;
    else
        file = new File;

    file->setFileName(name);

    if (!file->open(QIODevice::ReadOnly))
    {
        QString path = QFileDialog::getExistingDirectory(nullptr, "Choose input file location: " + info.fileName(), name);

        if (path.isEmpty())
            throw MessageLog({QtCriticalMsg, "File could not be opened.", name, nullptr, -1, -1});

        QString name = QDir(path).filePath(info.fileName());

        file->setFileName(name);

        if (!file->open(QIODevice::ReadOnly))
            throw MessageLog({QtCriticalMsg, "File could not be opened.", name, nullptr, -1, -1});
    }
}

SimulationItem::~SimulationItem()
{
    delete file;
}

TreeItem* SimulationItem::getModel() const
{
    return model;
}

#include "pdbsimulationlayer.h"
#include "xvgsimulationseries.h"

SimulationItem* SimulationItem::read(const QJsonObject& json, Session* session)
{
    QString name = json["File name"].toString();
    int first = json["First"].toInt();
    int last = json["Last"].toInt();
    int stride = json["Stride"].toInt();

    QFileInfo info(name);

    if (info.completeSuffix() == "pdb" || info.completeSuffix() == "pdb.gz")
        return new PDBSimulationLayer(name, session, first, last, stride);

    if (info.completeSuffix() == "xvg" || info.completeSuffix() == "xvg.gz")
        return new XVGSimulationSeries(name, session, first, last, stride);

    throw MessageLog({QtCriticalMsg, "File format not recognized.", name, nullptr, -1, -1});
}

void SimulationItem::write(QJsonObject& json) const
{
    json["File name"] = file->fileName();
    json["First"] = first;
    json["Last"] = last;
    json["Stride"] = stride;
}
