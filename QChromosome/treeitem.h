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
#include <QTextStream>

#include <memory>
#include "../QtChromosomeViz_v2/bartekm_code/common.h"

enum VizFlag
{
    NoFlags = 0x0,
    Selected = 0x1,
    VisibleInEditor = 0x2,
    VisibleInRenderer = 0x4
};

Q_DECLARE_FLAGS(VizFlags, VizFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(VizFlags)

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

    virtual void setFlag(VizFlag flag, bool on = true);

    virtual void read(const QJsonObject& json);
    virtual void write(QJsonObject& json) const;

    virtual void writePOVFrame(QTextStream &stream, std::shared_ptr<Frame> frame) const;
    virtual void writePOVFrames(QTextStream &stream, frameNumber_t fbeg, frameNumber_t fend) const;

protected:
    explicit TreeItem(const QVariantList &data, int acount, int ccount, TreeItem *parentItem);

    int a_offset, m_atomCount;
    int c_offset, m_chainCount;

    virtual void remove();
    virtual void shift(int da, int dc);

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

class Session;

class LayerItem : public TreeItem
{
public:
    explicit LayerItem(const QString& name, std::shared_ptr<SimulationLayerConcatenation> slc, Session* s, TreeItem *parentItem = 0);
    ~LayerItem();

    void write(QJsonObject& json) const;

protected:
    void remove();

private:
    std::shared_ptr<SimulationLayerConcatenation> layer;
    Session* session;
};

#include <QMatrix4x4>

struct VizCameraInstance
{
    QMatrix4x4 modelView;
    QMatrix4x4 projection;
    VizFlags flags = VisibleInEditor | VisibleInRenderer;
};

class Camera;

class CameraItem : public TreeItem
{
public:
    explicit CameraItem(const QString& name, Camera* cam, Session* s, TreeItem *parentItem = 0);
    ~CameraItem();

    QVector3D getPosition() const;
    void setPosition(const QVector3D& p);

    void setFlag(VizFlag flag, bool on = true);

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

    Camera* getCamera() const;

protected:
    void remove();

private:
    Camera* camera;
    Session* session;

friend class Camera;
};

#include <QVector3D>
#include <QRgb>

struct VizBallInstance
{
    QVector3D position;
    VizFlags flags = VisibleInEditor | VisibleInRenderer;
    float size = 1.0;
    float radius = 0.5;
    QRect label;
    int material = 0;
};

#include "material.h"

class AtomItem : public TreeItem
{
public:
    explicit AtomItem(const Atom& atom, int id, Session *s, TreeItem *parentItem = 0);
    ~AtomItem();

    void setLabel(const QString& l, const QRect& r);
    const QString& getLabel() const;

    void setCylinderRadius(float r);
    float getCylinderRadius() const;

    void setSphereRadius(float r);
    float getSphereRadius() const;

    void setMaterial(const Material* material);
    void setFlag(VizFlag flag, bool on = true);

    bool isSelected() const;
    const VizBallInstance& getInstance() const;

    QVector3D getPosition() const;

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

    void writePOVFrame(QTextStream &stream, std::shared_ptr<Frame> frame) const;
    void writePOVFrames(QTextStream &stream, frameNumber_t fbeg, frameNumber_t fend) const;

protected:
    void shift(int da, int dc);

private:
    int id;
    QString label;
    Session *session;
};

class ChainItem : public TreeItem
{
public:
    explicit ChainItem(const QString& name, std::pair<int, int> r, Session *s, TreeItem *parentItem = 0);
    ~ChainItem();

    void writePOVFrame(QTextStream &stream, std::shared_ptr<Frame> frame) const;
    void writePOVFrames(QTextStream &stream, frameNumber_t fbeg, frameNumber_t fend) const;

protected:
    void shift(int da, int dc);

private:
    int id;
    Session *session;
};

class ResidueItem : public TreeItem
{
public:
    explicit ResidueItem(int type, TreeItem *parentItem = 0);
    ~ResidueItem();
};

#endif // TREEITEM_H
