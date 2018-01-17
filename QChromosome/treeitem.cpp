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

#include <QVector3D>

QVector3D TreeItem::getPosition() const
{
    return QVector3D(0, 0, 0);
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
    {
        QVariantList u;

        for (auto i : t.value().toArray())
            u.append(QVariant::fromValue(MaterialBrowser::getMaterialById(i.toString())));

        m_itemData[5] = u;
    }

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

void TreeItem::writePOVFrame(std::ostream &stream, std::shared_ptr<Frame> frame, const Material* material, QSet<const Material*>& used) const
{
    for (const auto c : m_childItems)
    {
        auto tags = c->m_itemData[5].toList();

        if (!tags.empty())
            material = tags.last().value<Material*>();

        c->writePOVFrame(stream, frame, material, used);
    }
}

void TreeItem::writePOVFrames(std::ostream &stream, frameNumber_t fbeg, frameNumber_t fend) const
{

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

CameraItem::CameraItem(const QString &name, Camera *cam, TreeItem *parentItem) :
    TreeItem({name, NodeType::CameraObject, QVariant(), Visibility::Default, Visibility::Default, QVariant(), false}, parentItem),
    camera(cam)
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

QVector<VizBallInstance> AtomItem::buffer;

AtomItem::AtomItem(const Atom &atom, int id, TreeItem *parentItem) :
    TreeItem({QString("Atom.%1").arg(atom.id), NodeType::AtomObject, id, Visibility::Default, Visibility::Default, QVariant()}, parentItem),
    id(id)
{
    QIcon icon;
    icon.addPixmap(QPixmap(":/objects/atom"), QIcon::Normal);
    icon.addPixmap(QPixmap(":/objects/atom"), QIcon::Selected);
    decoration = icon;

    buffer[id].position[0] = atom.x;
    buffer[id].position[1] = atom.y;
    buffer[id].position[2] = atom.z;
}

AtomItem::~AtomItem()
{

}

VizBallInstance& AtomItem::getInstance()
{
    return buffer[id];
}

const QVector<VizBallInstance>& AtomItem::getBuffer()
{
    return buffer;
}

QVector3D AtomItem::getPosition() const
{
    auto buff = buffer[id].position;
    return QVector3D(buff[0], buff[1], buff[2]);
}

void AtomItem::resizeBuffer(int count)
{
    int offset = buffer.size();

    buffer.resize(offset + count);

    VizBallInstance dummy;
    dummy.flags = VISIBLE_FLAG;
    dummy.size = 1.f;

    std::fill(buffer.begin() + offset, buffer.end(), dummy);

    for (int i = offset; i < buffer.size(); i++)
        buffer[i].atomID = i;
}

void AtomItem::setFrame(std::shared_ptr<Frame> frame)
{
    assert(frame->atoms.size() == buffer.size());

    auto& atoms = frame->atoms;

    for (int i = 0; i < atoms.size(); i++)
    {
        auto& atom = atoms[i];
        auto* buff = buffer[i].position;
        buff[0] = atom.x;
        buff[1] = atom.y;
        buff[2] = atom.z;
    }
}

void AtomItem::setLabel(const QString& l)
{
    label = l;
}

const QString& AtomItem::getLabel() const
{
    return label;
}

void AtomItem::setRadius(float r)
{
    buffer[id].size = r;
}

float AtomItem::getRadius() const
{
    return buffer[id].size;
}

void AtomItem::setMaterial(const Material *material)
{
    unsigned int alpha = 255 * (1. - material->getTransparency());
    unsigned int code1 = (material->getColor().rgb() & 0xFFFFFF) | (alpha << 24);
    unsigned int code2 = material->getSpecularColor().rgba();
    float exponent = material->getSpecularExponent();

    auto& buff = buffer[id];
    buff.color = code1;
    buff.specularColor = code2;
    buff.specularExponent = exponent;
}

void AtomItem::setVisibility(bool visible)
{
    if (visible)
        buffer[id].flags |= VISIBLE_FLAG;
    else
        buffer[id].flags &= ~VISIBLE_FLAG;
}

void AtomItem::setSelected(bool selected)
{
    if (selected)
        buffer[id].flags |= SELECTED_FLAG;
    else
        buffer[id].flags &= ~SELECTED_FLAG;
}

void AtomItem::read(const QJsonObject &json)
{
    TreeItem::read(json);

    const QJsonObject object = json["Object"].toObject();

    auto rad = object.find("Radius");
    if (rad != object.end()) buffer[id].size = (*rad).toDouble();

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

    if (buffer[id].size != 1)
        object["Radius"] = buffer[id].size;

    if (!label.isEmpty())
        object["Label"] = label;

    json["Object"] = object;
}

#include "moviemaker.h"

constexpr QVector3D vec3(const Atom& a)
{
    return {a.x, a.y, a.z};
}

void AtomItem::writePOVFrame(std::ostream &stream, std::shared_ptr<Frame> frame, const Material* material, QSet<const Material *> &used) const
{
    if (!used.contains(material))
    {
        stream << *material;
        used.insert(material);
    }

    MovieMaker::addSphere(stream, vec3(frame->atoms[id - 1]), buffer[id].size, material);

    TreeItem::writePOVFrame(stream, frame, material, used);
}

ChainItem::ChainItem(const QString& name, std::pair<int, int> ran, TreeItem *parentItem) :
    TreeItem({name, NodeType::ChainObject, QVariant(), Visibility::Default, Visibility::Default, QVariant()}, parentItem),
    range(ran)
{
    QIcon icon;
    icon.addPixmap(QPixmap(":/objects/chain"), QIcon::Normal);
    icon.addPixmap(QPixmap(":/objects/chain"), QIcon::Selected);
    decoration = icon;
}

ChainItem::~ChainItem()
{

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
