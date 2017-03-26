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
#include "treemodel.h"

#include <QStringList>

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent),
      header(new TreeItem)
{

}

TreeModel::~TreeModel()
{
    delete header;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    return 5;
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
            switch (item->data(1).toInt())
            {
            case NodeType::LayerObject:
                return QIcon(":/objects/layer");
            case NodeType::ChainObject:
                return QIcon(":/lineedit/search");
            case NodeType::ResidueObject:
                return QIcon(":/lineedit/search");
            case NodeType::AtomObject:
                return QIcon(":/objects/atom");
            }
    }

    return QVariant();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
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

#include "defaults.h"

void appendSubmodel(const Atom *first, const Atom *last, unsigned int n, unsigned int offset, TreeItem *parent)
{
    TreeItem* root = new TreeItem({QString("Chain") + (n ? QString(".") + QString::number(n) : ""), NodeType::ChainObject, QVariant(), "On", "Tags"}, parent);

    QMap<int, TreeItem*> types;

    for (auto atom = first; atom != last; atom++)
    {
        int t = atom->type;

        if (!types.contains(t))
            types[t] = new TreeItem({Defaults::typename2label(t), NodeType::ResidueObject, QVariant(), "On", "Tags"}, root);

        types[t]->appendChild(new TreeItem({QString("Atom.%1").arg(atom->id), NodeType::AtomObject, atom->id - 1 + offset, "On", "Tags"}, types[t]));
    }

    for (auto t : types)
        root->appendChild(t);

    parent->appendChild(root);
}

#include <QBitArray>

void TreeModel::setupModelData(const std::vector<Atom> &atoms, std::vector<std::pair<int, int>> &connectedRanges, unsigned int n, unsigned int offset)
{
    QBitArray used(atoms.size(), false);

    TreeItem* root = new TreeItem({QString("Layer") + (n ? QString(".") + QString::number(n + 1) : ""), NodeType::LayerObject, n, "On", "Tags"}, header);

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
                types[t] = new TreeItem({Defaults::typename2label(t), NodeType::ResidueObject, QVariant(), "On", "Tags"}, root);

            types[t]->appendChild(new TreeItem({QString("Atom.%1").arg(atoms[i].id), NodeType::AtomObject, atoms[i].id - 1 + offset, "On", "Tags"}, types[t]));
        }

    for (auto t : types)
        root->appendChild(t);

    beginInsertRows(QModelIndex(), rowCount(), rowCount() + 1);

    header->appendChild(root);

    endInsertRows();
}
