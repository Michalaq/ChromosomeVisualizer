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

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include "treeitem.h"

class Material;
class Simulation;

enum VisibilityMode
{
    Editor = 3,
    Renderer = 4
};

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(QObject *parent = 0);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    void setupModelData(std::shared_ptr<SimulationLayerConcatenation> slc, unsigned int layer, unsigned int offset, bool init = true);
    const QVector<QPersistentModelIndex> &getIndices() const;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    void addCamera(Camera* camera);
    void setCurrentCamera(QModelIndex index);
    Camera* getCurrentCamera() const;

    void setMaterial(const QModelIndex& root, Material* m, int position = INT_MAX);
    Material* removeMaterial(const QModelIndex& root, int position);

    void setVisibility(const QModelIndex& index, Visibility v, VisibilityMode m);
    void setVisibility(const QModelIndexList& indices, Visibility v, VisibilityMode m);

    void setSelected(const QModelIndexList& indices, bool s);

    void setName(const QModelIndex& index, const QString& name);
    void setName(const QModelIndexList& indices, const QString& name);

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

public slots:
    void updateAttributes(const Material* m);

private:
    TreeItem *header;
    QVector<QPersistentModelIndex> indices;
    QPersistentModelIndex currentCamera;

    QString next_name() const;

    void propagateMaterial(const QModelIndex &root, const Material* m);
    void updateMaterial(const QModelIndex &root, const Material* m);

    void propagateVisibility(const QModelIndex &root, bool v);

    void propagateSelected(const QModelIndex &root, bool s);

signals:
    void propertyChanged();
    void attributeChanged();
};

#endif // TREEMODEL_H
