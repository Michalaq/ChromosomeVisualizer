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

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>

#include <memory>
#include "../QtChromosomeViz_v2/bartekm_code/common.h"

class Material;

class TreeItem
{
public:
    explicit TreeItem(const QVariantList &data = QVariantList(), TreeItem *parentItem = 0);
    virtual ~TreeItem();

    void appendChild(TreeItem *child);
    void prependChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool setData(int column, const QVariant& data);
    int row() const;
    TreeItem *parentItem();
    void removeRows(int row, int count);

    int selected_children_count = 0;
    int selected_tag_index = -1;
    QVariant decoration;

    virtual QVector3D getPosition() const;

    virtual void read(const QJsonObject& json);
    virtual void write(QJsonObject& json) const;

    virtual void writePOVFrame(std::ostream &stream, std::shared_ptr<Frame> frame, QSet<const Material *> &used) const;
    virtual void writePOVFrames(std::ostream &stream, frameNumber_t fbeg, frameNumber_t fend, QSet<const Material *> &used) const;

private:
    QList<TreeItem*> m_childItems;
    QList<QVariant> m_itemData;
    TreeItem *m_parentItem;
};

enum NodeType
{
    HeaderObject,
    LayerObject,
    ChainObject,
    ResidueObject,
    AtomObject,
    CameraObject
};

enum Visibility
{
    Default,
    On,
    Off
};

#include "../QtChromosomeViz_v2/bartekm_code/SimulationLayerConcatenation.h"

class LayerItem : public TreeItem
{
public:
    explicit LayerItem(const QString& name, std::shared_ptr<SimulationLayerConcatenation> slc, TreeItem *parentItem = 0);
    ~LayerItem();

    void write(QJsonObject& json) const;

private:
    std::shared_ptr<SimulationLayerConcatenation> layer;
};

class Camera;

class CameraItem : public TreeItem
{
public:
    explicit CameraItem(const QString& name, Camera* cam, TreeItem *parentItem = 0);
    ~CameraItem();

    QVector3D getPosition() const;
    void setPosition(const QVector3D& p);

    void write(QJsonObject& json) const;

    Camera* getCamera() const;

private:
    Camera* camera;
};

#include <QVector3D>

struct VizBallInstance
{
    QVector3D position;
    unsigned int flags;
    unsigned int atomID;
    unsigned int color;
    unsigned int specularColor;
    float specularExponent;
    float size;
    const Material* material;
};

enum VizBallFlags : unsigned int
{
    SELECTED_FLAG   = 1u << 0,
    VIE_FLAG        = 1u << 1,
    VIR_FLAG        = 1u << 2,
};

#include "material.h"

class AtomItem : public TreeItem
{
public:
    explicit AtomItem(const Atom& atom, int id, TreeItem *parentItem = 0);
    ~AtomItem();

    static const QVector<VizBallInstance>& getBuffer();
    static void resizeBuffer(int count);
    static void setFrame(std::shared_ptr<Frame> frame);

    void setLabel(const QString& l);
    const QString& getLabel() const;

    void setRadius(float r);
    float getRadius() const;

    void setMaterial(const Material* material);
    void setFlag(unsigned flag, bool on = true);

    QVector3D getPosition() const;

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

    void writePOVFrame(std::ostream &stream, std::shared_ptr<Frame> frame, QSet<const Material *> &used) const;
    void writePOVFrames(std::ostream &stream, frameNumber_t fbeg, frameNumber_t fend, QSet<const Material *> &used) const;

private:
    int id;
    static QVector<VizBallInstance> buffer;
    QString label;
};

class ChainItem : public TreeItem
{
public:
    explicit ChainItem(const QString& name, std::pair<int, int> r, TreeItem *parentItem = 0);
    ~ChainItem();

    static const QVector<unsigned> &getBuffer();

    void writePOVFrame(std::ostream &stream, std::shared_ptr<Frame> frame, QSet<const Material *> &used) const;
    void writePOVFrames(std::ostream &stream, frameNumber_t fbeg, frameNumber_t fend, QSet<const Material *> &used) const;

private:
    static QVector<unsigned> buffer;
    std::pair<int, int> range;
};

class ResidueItem : public TreeItem
{
public:
    explicit ResidueItem(const QString& name, TreeItem *parentItem = 0);
    ~ResidueItem();
};

#endif // TREEITEM_H
