#include "treeitem.h"

TreeItem::TreeItem(const QVariantList &data, TreeItem *parent) :
    cylinderRadius(0.5),
    sphereRadius(1.0),
    m_itemData(data),
    m_parentItem(parent)
{
    if (parent)
        parent->appendChild(this);
}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
    item->m_parentItem = this;
    m_childItems.append(item);
}

void TreeItem::prependChild(TreeItem *item)
{
    item->m_parentItem = this;
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

QVariant TreeItem::data(int column, int role) const
{
    if (column >= columnCount())
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
        return m_itemData.value(column);
    case Qt::UserRole:
        return selected_children_count;
    case Qt::UserRole + 1:
        return selected_tag_index;
    default:
        return QVariant();
    }
}

bool TreeItem::setData(int column, const QVariant &value, int role)
{
    if (column >= columnCount())
        return false;

    switch (role)
    {
    case Qt::EditRole:
        m_itemData.replace(column, value);
        break;
    case Qt::UserRole + 1:
        selected_tag_index = value.toInt();
        break;
    }

    return true;
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
    for (int i = 0; i < count; i++)
    {
        auto item = m_childItems.takeAt(row);
        item->remove();
        delete item;
    }
}

QVector3D TreeItem::getPosition() const
{
    return QVector3D(0, 0, 0);
}

void TreeItem::setSelected(bool on)
{
    auto root = m_parentItem;

    while (root)
    {
        root->selected_children_count += on ? 1 : -1;
        root = root->m_parentItem;
    }

    setFlag(Selected, on);
}

void TreeItem::setMaterial(const Material* material)
{
    for (const auto c : m_childItems)
        if (c->data(5).toList().isEmpty())
            c->setMaterial(material);
}

void TreeItem::setFlag(VizFlag flag, bool on)
{
    if (flag == Selected)
        for (auto c : m_childItems)
            c->setFlag(flag, on);
}

#include <QJsonObject>
#include <QJsonArray>

void TreeItem::read(const QJsonObject& json, const MaterialListModel* mlm, Material* mat, bool ve, bool vr)
{
    const QJsonObject object = json["Object"].toObject();

    auto vie = object.find("Visible in editor");

    if (vie != object.end())
        m_itemData[3] = (ve = vie.value().toBool()) ? On : Off;

    auto vir = object.find("Visible in renderer");

    if (vir != object.end())
        m_itemData[4] = (vr = vir.value().toBool()) ? On : Off;

    auto cr = object.find("Cylinder radius");

    if (cr != object.end())
        setCylinderRadius(cr.value().toDouble());

    auto sr = object.find("Sphere radius");

    if (sr != object.end())
        setSphereRadius(sr.value().toDouble());

    auto t = object.find("Tags");

    if (t != object.end())
    {
        QVariantList tags;

        for (auto i : t.value().toArray())
            tags.append(QVariant::fromValue(mat = mlm->getMaterialById(i.toString())));

        m_itemData[5] = tags;
    }

    const QJsonObject children = json["Descendants"].toObject();

    for (int i = 0; i < m_childItems.size(); i++)
        m_childItems[i]->read(children[QString::number(i)].toObject(), mlm, mat, ve, vr);
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

    if (m_parentItem)
    {
        if (m_parentItem->cylinderRadius != cylinderRadius)
            object["Cylinder radius"] = cylinderRadius;

        if (m_parentItem->sphereRadius != sphereRadius)
            object["Sphere radius"] = sphereRadius;
    }

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

void TreeItem::setCylinderRadius(float r)
{
    cylinderRadius = r;

    for (auto c : m_childItems)
        c->setCylinderRadius(r);
}

float TreeItem::getCylinderRadius() const
{
    return cylinderRadius;
}

void TreeItem::setSphereRadius(float r)
{
    sphereRadius = r;

    for (auto c : m_childItems)
        c->setSphereRadius(r);
}

float TreeItem::getSphereRadius() const
{
    return sphereRadius;
}

QList<Material*> TreeItem::selectedTags() const
{
    if (selected_tag_index == -1)
        return {};
    else
        return {m_itemData[5].toList()[selected_tag_index].value<Material*>()};
}

void TreeItem::remove()
{

}

#include "session.h"

LayerItem::LayerItem(const QString &name, SimulationLayer* l, TreeItem *parentItem) :
    TreeItem({name, NodeType::LayerObject, QVariant(), Visibility::Default, Visibility::Default, QVariant()}, parentItem),
    layer(l)
{

}

LayerItem::~LayerItem()
{

}

QVariant LayerItem::icon;

QVariant LayerItem::data(int column, int role) const
{
    switch (role)
    {
    case Qt::DecorationRole:
        if (!icon.isValid())
        {
            QIcon icon;
            icon.addPixmap(QPixmap(":/create/layer"), QIcon::Normal);
            icon.addPixmap(QPixmap(":/create/layer"), QIcon::Selected);
            this->icon = icon;
        }

        return icon;
    default:
        return TreeItem::data(column, role);
    }
}

void LayerItem::write(QJsonObject &json) const
{
    TreeItem::write(json);

    QJsonObject object;

    if (json.contains("Object"))
        object = json["Object"].toObject();

    object["class"] = "Layer";

    layer->write(object);

    json["Object"] = object;
}

void LayerItem::remove()
{
    layer->remove();
    delete layer;
}

#include "camera.h"

CameraItem::CameraItem(const QString &name, Camera *cam, Session *s, TreeItem *parentItem) :
    TreeItem({name, NodeType::CameraObject, QVariant(), Visibility::Default, Visibility::Default, QVariant(), false}, parentItem),
    camera(cam),
    session(s)
{
    camera->action->setText(name);
}

CameraItem::~CameraItem()
{

}

QVariant CameraItem::icon;

QVariant CameraItem::data(int column, int role) const
{
    switch (role)
    {
    case Qt::DecorationRole:
        if (!icon.isValid())
        {
            QIcon icon;
            icon.addPixmap(QPixmap(":/create/film camera"), QIcon::Normal);
            icon.addPixmap(QPixmap(":/create/film camera"), QIcon::Selected);
            this->icon = icon;
        }

        return icon;
    default:
        return TreeItem::data(column, role);
    }
}

bool CameraItem::setData(int column, const QVariant &value, int role)
{
    if (column == 0)
        camera->action->setText(value.toString());

    return TreeItem::setData(column, value, role);
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
    session->cameraBuffer[camera->id].flags.setFlag(flag, on);

    TreeItem::setFlag(flag, on);
}

void CameraItem::read(const QJsonObject& json, const MaterialListModel* mlm, Material* mat, bool ve, bool vr)
{
    TreeItem::read(json, mlm, mat, ve, vr);

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

void CameraItem::remove()
{
    session->userCameras.removeOne(camera);
    session->cameraBuffer.remove(camera->id, 1);

    if (data(6).toBool())
        session->simulation->getModel()->setCurrentCamera(QModelIndex());

    camera->deleteLater();
}

AtomItem::AtomItem(uint serial, const QByteArray &name, Session *s, TreeItem *parentItem) :
    TreeItem({name + "." + QString::number(serial), NodeType::AtomObject, QVariant(), Visibility::Default, Visibility::Default, QVariant()}, parentItem),
    id(s->atomBuffer.append({})),
    session(s)
{

}

AtomItem::~AtomItem()
{

}

QVariant AtomItem::icon;

QVariant AtomItem::data(int column, int role) const
{
    switch (role)
    {
    case Qt::DecorationRole:
        if (!icon.isValid())
        {
            QIcon icon;
            icon.addPixmap(QPixmap(":/create/atom"), QIcon::Normal);
            icon.addPixmap(QPixmap(":/create/atom"), QIcon::Selected);
            this->icon = icon;
        }

        return icon;
    default:
        return TreeItem::data(column, role);
    }
}

int AtomItem::getId() const
{
    return id;
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

void AtomItem::setCylinderRadius(float r)
{
    session->atomBuffer[id].radius = r;

    TreeItem::setCylinderRadius(r);
}

void AtomItem::setSphereRadius(float r)
{
    session->atomBuffer[id].size = r;

    TreeItem::setSphereRadius(r);
}

void AtomItem::setMaterial(const Material *material)
{
    session->atomBuffer[id].material = material->getIndex();

    TreeItem::setMaterial(material);
}

void AtomItem::setFlag(VizFlag flag, bool on)
{
    session->atomBuffer[id].flags.setFlag(flag, on);

    TreeItem::setFlag(flag, on);
}

void AtomItem::read(const QJsonObject& json, const MaterialListModel* mlm, Material* mat, bool ve, bool vr)
{
    TreeItem::read(json, mlm, mat, ve, vr);

    const QJsonObject object = json["Object"].toObject();

    auto rad = object.find("Radius");
    if (rad != object.end()) session->atomBuffer[id].size = (*rad).toDouble();

    auto lab = object.find("Label");
    if (lab != object.end()) label = (*lab).toString();

    session->atomBuffer[id].flags.setFlag(VisibleInEditor, ve);
    session->atomBuffer[id].flags.setFlag(VisibleInRenderer, vr);
    session->atomBuffer[id].material = mat->getIndex();
}

void AtomItem::write(QJsonObject &json) const
{
    TreeItem::write(json);

    QJsonObject object;

    if (json.contains("Object"))
        object = json["Object"].toObject();

    object["class"] = "Atom";

    if (!label.isEmpty())
        object["Label"] = label;

    if (object.size() > 1)
        json["Object"] = object;
}

ChainItem::ChainItem(const QByteArray &chainID, Session *s, TreeItem *parentItem) :
    TreeItem({chainID, NodeType::ChainObject, s->chainCount++, Visibility::Default, Visibility::Default, QVariant()}, parentItem)
{

}

ChainItem::~ChainItem()
{

}

QVariant ChainItem::icon;

QVariant ChainItem::data(int column, int role) const
{
    switch (role)
    {
    case Qt::DecorationRole:
        if (!icon.isValid())
        {
            QIcon icon;
            icon.addPixmap(QPixmap(":/create/chain"), QIcon::Normal);
            icon.addPixmap(QPixmap(":/create/chain"), QIcon::Selected);
            this->icon = icon;
        }

        return icon;
    default:
        return TreeItem::data(column, role);
    }
}

ResidueItem::ResidueItem(const QByteArray &resName, Session* s, TreeItem *parentItem) :
    TreeItem({resName, NodeType::ResidueObject, s->residueCount.contains(resName) ? s->residueCount[resName] : s->residueCount.count(), Visibility::Default, Visibility::Default, QVariant()}, parentItem)
{
    if (!s->residueCount.contains(resName))
        s->residueCount.insert(resName, s->residueCount.count());
}

ResidueItem::~ResidueItem()
{

}

QVariant ResidueItem::icon;

QVariant ResidueItem::data(int column, int role) const
{
    switch (role)
    {
    case Qt::DecorationRole:
        if (!icon.isValid())
        {
            QIcon icon;
            icon.addPixmap(QPixmap(":/create/residue"), QIcon::Normal);
            icon.addPixmap(QPixmap(":/create/residue"), QIcon::Selected);
            this->icon = icon;
        }

        return icon;
    default:
        return TreeItem::data(column, role);
    }
}

ChartItem::ChartItem(const QString &name, SimulationSeries* s, TreeItem *parentItem) :
    TreeItem({name, NodeType::ChartObject, QVariant(), Visibility::Default, Visibility::Default, QVariant()}, parentItem),
    series(s)
{

}

ChartItem::~ChartItem()
{

}

QVariant ChartItem::icon;

QVariant ChartItem::data(int column, int role) const
{
    switch (role)
    {
    case Qt::DecorationRole:
        if (!icon.isValid())
        {
            QIcon icon;
            icon.addPixmap(QPixmap(":/create/database"), QIcon::Normal);
            icon.addPixmap(QPixmap(":/create/database"), QIcon::Selected);
            this->icon = icon;
        }

        return icon;
    default:
        return TreeItem::data(column, role);
    }
}

void ChartItem::write(QJsonObject &json) const
{
    TreeItem::write(json);

    QJsonObject object;

    if (json.contains("Object"))
        object = json["Object"].toObject();

    object["class"] = "Layer";

    series->write(object);

    json["Object"] = object;
}

void ChartItem::remove()
{
    series->remove();
    delete series;
}

SeriesItem::SeriesItem(QtCharts::QLineSeries* s, TreeItem *parentItem) :
    TreeItem({s->name(), NodeType::SeriesObject, QVariant(), Visibility::Default, Visibility::Default, QVariant()}, parentItem),
    series(s)
{

}

SeriesItem::~SeriesItem()
{

}

QVariant SeriesItem::icon;

QVariant SeriesItem::data(int column, int role) const
{
    switch (role)
    {
    case Qt::DecorationRole:
        if (!icon.isValid())
        {
            QIcon icon;
            icon.addPixmap(QPixmap(":/create/area chart"), QIcon::Normal);
            icon.addPixmap(QPixmap(":/create/area chart"), QIcon::Selected);
            this->icon = icon;
        }

        return icon;
    default:
        return TreeItem::data(column, role);
    }
}

bool SeriesItem::setData(int column, const QVariant &value, int role)
{
    if (column == 0)
        series->setName(value.toString());

    return TreeItem::setData(column, value, role);
}
