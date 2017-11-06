#include "treeitem.h"
#include "treemodel.h"

#include <QStringList>

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent),
      header(new TreeItem({"Model", "", "", "", "", "Tags"}))
{

}

TreeModel::~TreeModel()
{
    delete header;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    return 6;
}

#include <QIcon>

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    switch (role)
    {
    case Qt::DisplayRole:
        return item->data(index.column());
    case Qt::DecorationRole:
        if (index.column() == 0)
        {
            QIcon icon;

            switch (item->data(1).toInt())
            {
            case NodeType::LayerObject:
                icon.addPixmap(QPixmap(":/objects/layer"), QIcon::Normal);
                icon.addPixmap(QPixmap(":/objects/layer"), QIcon::Selected);
                break;
            case NodeType::ChainObject:
                icon.addPixmap(QPixmap(":/objects/chain"), QIcon::Normal);
                icon.addPixmap(QPixmap(":/objects/chain"), QIcon::Selected);
                break;
            case NodeType::ResidueObject:
                icon.addPixmap(QPixmap(":/objects/residue"), QIcon::Normal);
                icon.addPixmap(QPixmap(":/objects/residue"), QIcon::Selected);
                break;
            case NodeType::AtomObject:
                icon.addPixmap(QPixmap(":/objects/atom"), QIcon::Normal);
                icon.addPixmap(QPixmap(":/objects/atom"), QIcon::Selected);
                break;
            case NodeType::CameraObject:
                icon.addPixmap(QPixmap(":/dialogs/film camera"), QIcon::Normal);
                icon.addPixmap(QPixmap(":/dialogs/film camera"), QIcon::Selected);
                break;
            }

            return icon;
        }
    case Qt::UserRole:
        return item->selected_children_count;
    case Qt::UserRole + 1:
        return item->selected_tag_index;
    }

    return QVariant();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    switch (role)
    {
    case Qt::UserRole:
        item->selected_children_count = value.toInt();
        return true;
    case Qt::UserRole + 1:
        item->selected_tag_index = value.toInt();
        return true;
    default:
        return item->setData(index.column(), value);
    }
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index) | (index.column() == 0 ? Qt::ItemIsEditable : Qt::NoItemFlags);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return header->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = header;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == header)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = header;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void dumpModel(const QAbstractItemModel* model, const QModelIndex& root, QVector<QPersistentModelIndex>& id)
{
    auto v = root.sibling(root.row(), 2).data();

    if (v.canConvert<uint>())
        id[v.toUInt()] = root;

    for (int r = 0; r < model->rowCount(root); r++)
        dumpModel(model, model->index(r, 0, root), id);
}

#include "defaults.h"

void appendSubmodel(const Atom *first, const Atom *last, unsigned int n, unsigned int offset, TreeItem *parent, bool init)
{
    TreeItem* root = new TreeItem({QString("Chain") + (n ? QString(".") + QString::number(n) : ""), NodeType::ChainObject, QVariant(), Visibility::Default, Visibility::Default, QVariant()}, parent);

    QMap<int, TreeItem*> types;

    for (auto atom = first; atom != last; atom++)
    {
        int t = atom->type;

        if (!types.contains(t))
            types[t] = new TreeItem({Defaults::typename2label(t), NodeType::ResidueObject, QVariant(), Visibility::Default, Visibility::Default, init ? Defaults::typename2color(t) : QVariant()}, root);

        types[t]->appendChild(new TreeItem({QString("Atom.%1").arg(atom->id), NodeType::AtomObject, atom->id - 1 + offset, Visibility::Default, Visibility::Default, QVariant()}, types[t]));
    }

    for (auto t : types)
        root->appendChild(t);

    parent->appendChild(root);
}

#include <QBitArray>

void TreeModel::setupModelData(std::shared_ptr<SimulationLayerConcatenation> slc, unsigned int layer, unsigned int offset, bool init)
{
    auto f = slc->getFrame(0);
    auto& atoms = f->atoms;
    auto& connectedRanges = f->connectedRanges;

    QBitArray used(atoms.size(), false);

    TreeItem* root = new LayerItem(QString("Layer") + (layer ? QString(".") + QString::number(layer + 1) : ""), slc, header);

    unsigned int i = 0;

    for (auto range : connectedRanges)
    {
        used.fill(true, range.first - 1, range.second);
        appendSubmodel(&atoms[range.first - 1], &atoms[range.second - 1] + 1, i++, offset, root, init);
    }

    QMap<int, TreeItem*> types;

    for (auto i = 0; i < atoms.size(); i++)
        if (!used.testBit(i))
        {
            int t = atoms[i].type;

            if (!types.contains(t))
                types[t] = new TreeItem({Defaults::typename2label(t), NodeType::ResidueObject, QVariant(), Visibility::Default, Visibility::Default, init ? Defaults::typename2color(t) : QVariant()}, root);

            types[t]->appendChild(new TreeItem({QString("Atom.%1").arg(atoms[i].id), NodeType::AtomObject, atoms[i].id - 1 + offset, Visibility::Default, Visibility::Default, QVariant()}, types[t]));
        }

    for (auto t : types)
        root->appendChild(t);

    beginInsertRows(QModelIndex(), 0, 0);
    header->prependChild(root);
    endInsertRows();

    indices.resize(offset + atoms.size() + 1);
    dumpModel(this, index(0, 0), indices);
}

const QVector<QPersistentModelIndex>& TreeModel::getIndices() const
{
    return indices;
}

bool TreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    for (int i = row; i < row + count; i++)
    {
        if (data(index(i, 1, parent)).toInt() == NodeType::CameraObject)
        {
            beginRemoveRows(parent, i, i);
            (parent.isValid() ? static_cast<LayerItem*>(parent.internalPointer()) : header)->removeRows(i, 1);
            endRemoveRows();
        }
    }
}

#include <QRegularExpression>
#include <set>

QString TreeModel::next_name() const
{
    static const QRegularExpression re("^Camera(.(?<label>[1-9][0-9]*))?$");

    std::set<int> used;

    for (int r = 0; r < rowCount(); r++)
    {
        QRegularExpressionMatch match = re.match(index(r, 0).data().toString());

        if (match.hasMatch())
            used.insert(match.captured("label").toInt());
    }

    int i = 0;

    for (auto j = used.begin(); j != used.end() && i == *j; i++, j++);

    return i ? QString("Camera.") + QString::number(i) : "Camera";
}

void TreeModel::addCamera()
{
    TreeItem* root = new CameraItem(next_name(), header);

    beginInsertRows(QModelIndex(), 0, 0);
    header->prependChild(root);
    endInsertRows();
}

#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"
#include <QJsonArray>
#include <QJsonObject>

void TreeModel::read(std::shared_ptr<Simulation> simulation, const QJsonObject &json)
{
    const QJsonObject children = json["Descendants"].toObject();

    for (auto child = children.end() - 1; child != children.begin() - 1; child--)
    {
        const QJsonObject object = child.value().toObject()["Object"].toObject();

        if (object["class"] == "Layer")
        {
            auto simulationLayer = std::make_shared<SimulationLayerConcatenation>();
            simulationLayer->read(object["paths"].toArray());

            simulation->addSimulationLayerConcatenation(simulationLayer, false);
        }

        if (object["class"] == "Camera")
            addCamera();
    }

    header->read(json);
}

void TreeModel::write(QJsonObject &json) const
{
    header->write(json);
}
