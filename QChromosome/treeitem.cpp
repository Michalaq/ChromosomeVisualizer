/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "treeitem.h"

TreeItem::TreeItem(const QVariantList &data, TreeItem *parent)
{
    m_parentItem = parent;
    m_itemData = data;
}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
    m_childItems.append(item);
}

void TreeItem::prependChild(TreeItem *item)
{
    m_childItems.prepend(item);
}

TreeItem *TreeItem::child(int row)
{
    return m_childItems.value(row);
}

int TreeItem::childCount() const
{
    return m_childItems.count();
}

int TreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant TreeItem::data(int column) const
{
    return m_itemData.value(column);
}

bool TreeItem::setData(int column, const QVariant& data)
{
    if (column < columnCount())
    {
        m_itemData[column] = data;
        return true;
    }
    else
        return false;
}

TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

void TreeItem::removeRows(int row, int count)
{
    for (int i = row; i < row + count; i++)
        delete m_childItems.takeAt(i);
}

QVector3D TreeItem::getPosition() const
{
    return QVector3D(0, 0, 0);
}

void TreeItem::setFlag(VizFlag flag, bool on)
{
    ;
}

#include <QJsonObject>
#include <QJsonArray>
#include "materialbrowser.h"

void TreeItem::read(const QJsonObject &json)
{
    const QJsonObject object = json["Object"].toObject();

    auto vie = object.find("Visible in editor");

    if (vie != object.end())
        m_itemData[3] = vie.value().toBool() ? On : Off;

    auto vir = object.find("Visible in renderer");

    if (vir != object.end())
        m_itemData[4] = vir.value().toBool() ? On : Off;

    auto t = object.find("Tags");

    if (t != object.end())
        m_itemData[5] = t.value().toArray().toVariantList();

    const QJsonObject children = json["Descendants"].toObject();

    for (auto child = children.begin(); child != children.end(); child++)
        m_childItems[child.key().toInt()]->read(child.value().toObject());
}

void TreeItem::write(QJsonObject &json) const
{
    QJsonObject object;

    auto vie = (Visibility)m_itemData.value(3).toInt();

    if (vie != Default)
        object["Visible in editor"] = vie == On;

    auto vir = (Visibility)m_itemData.value(4).toInt();

    if (vir != Default)
        object["Visible in renderer"] = vir == On;

    auto t = m_itemData.value(5).toList();

    if (!t.empty())
    {
        QJsonArray u;

        for (auto i : t)
            u.append(i.value<Material*>()->getId().toString());

        object["Tags"] = u;
    }

    if (!object.empty())
        json["Object"] = object;

    QJsonObject children;

    for (int r = 0; r < m_childItems.size(); r++)
    {
        QJsonObject child;

        m_childItems[r]->write(child);

        if (!child.empty())
            children[QString::number(r)] = child;
    }

    if (!children.empty())
        json["Descendants"] = children;
}

void TreeItem::writePOVFrame(QTextStream &stream, std::shared_ptr<Frame> frame) const
{
    for (const auto c : m_childItems)
        c->writePOVFrame(stream, frame);
}

void TreeItem::writePOVFrames(QTextStream &stream, frameNumber_t fbeg, frameNumber_t fend) const
{
    for (const auto c : m_childItems)
        c->writePOVFrames(stream, fbeg, fend);
}

LayerItem::LayerItem(const QString &name, std::shared_ptr<SimulationLayerConcatenation> slc, TreeItem *parentItem) :
    TreeItem({name, NodeType::LayerObject, QVariant(), Visibility::Default, Visibility::Default, QVariant()}, parentItem),
    layer(slc)
{
    QIcon icon;
    icon.addPixmap(QPixmap(":/objects/layer"), QIcon::Normal);
    icon.addPixmap(QPixmap(":/objects/layer"), QIcon::Selected);
    decoration = icon;
}

LayerItem::~LayerItem()
{

}

void LayerItem::write(QJsonObject &json) const
{
    TreeItem::write(json);

    QJsonObject object;

    if (json.contains("Object"))
        object = json["Object"].toObject();

    QJsonArray simulationLayer;
    layer->write(simulationLayer);

    object["class"] = "Layer";
    object["paths"] = simulationLayer;

    json["Object"] = object;
}

#include "camera.h"
#include "session.h"

CameraItem::CameraItem(const QString &name, Camera *cam, Session *s, TreeItem *parentItem) :
    TreeItem({name, NodeType::CameraObject, QVariant(), Visibility::Default, Visibility::Default, QVariant(), false}, parentItem),
    camera(cam),
    id(cam->id),
    session(s)
{
    QIcon icon;
    icon.addPixmap(QPixmap(":/dialogs/film camera"), QIcon::Normal);
    icon.addPixmap(QPixmap(":/dialogs/film camera"), QIcon::Selected);
    decoration = icon;
}

CameraItem::~CameraItem()
{

}

QVector3D CameraItem::getPosition() const
{
    return camera->getPosition();
}

void CameraItem::setPosition(const QVector3D& p)
{
    camera->setPosition(p);
}

void CameraItem::setFlag(VizFlag flag, bool on)
{
    session->cameraBuffer[id].flags.setFlag(flag, on);
}

void CameraItem::read(const QJsonObject &json)
{
    TreeItem::read(json);

    const QJsonObject object = json["Object"].toObject();
    camera->read(object);
}

void CameraItem::write(QJsonObject &json) const
{
    TreeItem::write(json);

    QJsonObject object;

    if (json.contains("Object"))
        object = json["Object"].toObject();

    object["class"] = "Camera";
    camera->write(object);

    json["Object"] = object;
}

Camera* CameraItem::getCamera() const
{
    return camera;
}

AtomItem::AtomItem(const Atom &atom, int id, Session *s, TreeItem *parentItem) :
    TreeItem({QString("Atom.%1").arg(atom.id), NodeType::AtomObject, id, Visibility::Default, Visibility::Default, QVariant()}, parentItem),
    id(id),
    session(s)
{
    QIcon icon;
    icon.addPixmap(QPixmap(":/objects/atom"), QIcon::Normal);
    icon.addPixmap(QPixmap(":/objects/atom"), QIcon::Selected);
    decoration = icon;

    session->atomBuffer[id].position = QVector3D(atom.x, atom.y, atom.z);
}

AtomItem::~AtomItem()
{

}

QVector3D AtomItem::getPosition() const
{
    return session->atomBuffer[id].position;
}

void AtomItem::setLabel(const QString& l, const QRect &r)
{
    label = l;
    session->atomBuffer[id].label = r;
}

const QString& AtomItem::getLabel() const
{
    return label;
}

void AtomItem::setRadius(float r)
{
    session->atomBuffer[id].size = r;
}

float AtomItem::getRadius() const
{
    return session->atomBuffer[id].size;
}

void AtomItem::setMaterial(const Material *material)
{
    session->atomBuffer[id].material = material->getIndex();
}

void AtomItem::setFlag(VizFlag flag, bool on)
{
    session->atomBuffer[id].flags.setFlag(flag, on);
}

bool AtomItem::isSelected() const
{
    return session->atomBuffer[id].flags.testFlag(Selected);
}

const VizBallInstance& AtomItem::getInstance() const
{
    return session->atomBuffer[id];
}

void AtomItem::read(const QJsonObject &json)
{
    TreeItem::read(json);

    const QJsonObject object = json["Object"].toObject();

    auto rad = object.find("Radius");
    if (rad != object.end()) session->atomBuffer[id].size = (*rad).toDouble();

    auto lab = object.find("Label");
    if (lab != object.end()) label = (*lab).toString();
}

void AtomItem::write(QJsonObject &json) const
{
    TreeItem::write(json);

    QJsonObject object;

    if (json.contains("Object"))
        object = json["Object"].toObject();

    object["class"] = "Atom";

    if (session->atomBuffer[id].size != 1)
        object["Radius"] = session->atomBuffer[id].size;

    if (!label.isEmpty())
        object["Label"] = label;

    if (object.size() > 1)
        json["Object"] = object;
}

#include "moviemaker.h"

constexpr QVector3D vec3(const Atom& a)
{
    return {a.x, a.y, a.z};
}

void AtomItem::writePOVFrame(QTextStream &stream, std::shared_ptr<Frame> frame) const
{
    const auto& atom = session->atomBuffer[id];

    if (atom.flags.testFlag(VisibleInRenderer))
        MovieMaker::addSphere(stream, vec3(frame->atoms[id]), atom.size, atom.material);

    TreeItem::writePOVFrame(stream, frame);
}

void AtomItem::writePOVFrames(QTextStream &stream, frameNumber_t fbeg, frameNumber_t fend) const
{
    const auto& atom = session->atomBuffer[id];

    if (atom.flags.testFlag(VisibleInRenderer))
        MovieMaker::addSphere1(stream, id, atom.size, atom.material);

    TreeItem::writePOVFrames(stream, fbeg, fend);
}

ChainItem::ChainItem(const QString& name, std::pair<int, int> r, Session *s, TreeItem *parentItem) :
    TreeItem({name, NodeType::ChainObject, QVariant(), Visibility::Default, Visibility::Default, QVariant()}, parentItem),
    range(r),
    session(s)
{
    QIcon icon;
    icon.addPixmap(QPixmap(":/objects/chain"), QIcon::Normal);
    icon.addPixmap(QPixmap(":/objects/chain"), QIcon::Selected);
    decoration = icon;

    session->chainBuffer.append({r.first, r.second - r.first});
}

ChainItem::~ChainItem()
{

}

void ChainItem::writePOVFrame(QTextStream &stream, std::shared_ptr<Frame> frame) const
{
    TreeItem::writePOVFrame(stream, frame);

    for (int i = range.first; i < range.second - 1; i++)
    {
        const auto& first = session->atomBuffer[i];
        const auto& second = session->atomBuffer[i + 1];

        if ((first.flags & second.flags).testFlag(VisibleInRenderer))
            MovieMaker::addCylinder(stream, vec3(frame->atoms[i]), vec3(frame->atoms[i + 1]), first.size / 2, second.size / 2, first.material, second.material);
    }
}

void ChainItem::writePOVFrames(QTextStream &stream, frameNumber_t fbeg, frameNumber_t fend) const
{
    TreeItem::writePOVFrames(stream, fbeg, fend);

    for (int i = range.first; i < range.second - 1; i++)
    {
        const auto& first = session->atomBuffer[i];
        const auto& second = session->atomBuffer[i + 1];

        if ((first.flags & second.flags).testFlag(VisibleInRenderer))
            MovieMaker::addCylinder1(stream, i, i + 1, first.size / 2, second.size / 2, first.material, second.material);
    }
}

ResidueItem::ResidueItem(const QString& name, TreeItem *parentItem) :
    TreeItem({name, NodeType::ResidueObject, QVariant(), Visibility::Default, Visibility::Default, QVariant()}, parentItem)
{
    QIcon icon;
    icon.addPixmap(QPixmap(":/objects/residue"), QIcon::Normal);
    icon.addPixmap(QPixmap(":/objects/residue"), QIcon::Selected);
    decoration = icon;
}

ResidueItem::~ResidueItem()
{

}
