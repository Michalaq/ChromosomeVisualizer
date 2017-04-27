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
            }

            return icon;
        }
    case Qt::UserRole:
        return item->selected_children_count;
    }

    return QVariant();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if (role == Qt::UserRole)
    {
        item->selected_children_count = value.toInt();
        return true;
    }

    return item->setData(index.column(), value);
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

void dumpModel(const QAbstractItemModel* model, const QModelIndex& root, QVector<QModelIndex>& id)
{
    auto v = root.sibling(root.row(), 2).data();

    if (v.canConvert<uint>())
        id[v.toUInt()] = root;

    for (int r = 0; r < model->rowCount(root); r++)
        dumpModel(model, root.child(r, 0), id);
}

#include "defaults.h"

void appendSubmodel(const Atom *first, const Atom *last, unsigned int n, unsigned int offset, TreeItem *parent)
{
    TreeItem* root = new TreeItem({QString("Chain") + (n ? QString(".") + QString::number(n) : ""), NodeType::ChainObject, QVariant(), Visibility::Default, Visibility::Default, "<<tu będą tagi>>"}, parent);

    QMap<int, TreeItem*> types;

    for (auto atom = first; atom != last; atom++)
    {
        int t = atom->type;

        if (!types.contains(t))
            types[t] = new TreeItem({Defaults::typename2label(t), NodeType::ResidueObject, QVariant(), Visibility::Default, Visibility::Default, "<<tu będą tagi>>"}, root);

        types[t]->appendChild(new TreeItem({QString("Atom.%1").arg(atom->id), NodeType::AtomObject, atom->id - 1 + offset, Visibility::Default, Visibility::Default, "<<tu będą tagi>>"}, types[t]));
    }

    for (auto t : types)
        root->appendChild(t);

    parent->appendChild(root);
}

#include <QBitArray>

void TreeModel::setupModelData(const std::vector<Atom> &atoms, std::vector<std::pair<int, int>> &connectedRanges, unsigned int n, unsigned int offset)
{
    QBitArray used(atoms.size(), false);

    TreeItem* root = new TreeItem({QString("Layer") + (n ? QString(".") + QString::number(n + 1) : ""), NodeType::LayerObject, n, Visibility::Default, Visibility::Default, "<<tu będą tagi>>"}, header);

    unsigned int i = 0;

    for (auto range : connectedRanges)
    {
        used.fill(true, range.first - 1, range.second);
        appendSubmodel(&atoms[range.first - 1], &atoms[range.second - 1] + 1, i++, offset, root);
    }

    QMap<int, TreeItem*> types;

    for (auto i = 0; i < atoms.size(); i++)
        if (!used.testBit(i))
        {
            int t = atoms[i].type;

            if (!types.contains(t))
                types[t] = new TreeItem({Defaults::typename2label(t), NodeType::ResidueObject, QVariant(), Visibility::Default, Visibility::Default, "<<tu będą tagi>>"}, root);

            types[t]->appendChild(new TreeItem({QString("Atom.%1").arg(atoms[i].id), NodeType::AtomObject, atoms[i].id - 1 + offset, Visibility::Default, Visibility::Default, "<<tu będą tagi>>"}, types[t]));
        }

    for (auto t : types)
        root->appendChild(t);

    beginInsertRows(QModelIndex(), rowCount(), rowCount() + 1);

    header->appendChild(root);

    endInsertRows();

    indices.resize(atoms.size() + 1);
    dumpModel(this, index(0, 0), indices);
}

const QVector<QModelIndex>& TreeModel::getIndices() const
{
    return indices;
}
