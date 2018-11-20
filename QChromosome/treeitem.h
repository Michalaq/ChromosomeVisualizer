#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>

enum VizFlag
{
    NoFlags = 0x0,
    Selected = 0x1,
    VisibleInEditor = 0x2,
    VisibleInRenderer = 0x4
};

Q_DECLARE_FLAGS(VizFlags, VizFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(VizFlags)

#include "material.h"
#include "materialbrowser.h"

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

    virtual void setSelected(bool on = true);
    virtual void setMaterial(const Material* material);
    virtual void setFlag(VizFlag flag, bool on = true);

    virtual void read(const QJsonObject& json, const MaterialListModel* mlm, Material* mat = Material::getDefault(), bool ve = true, bool vr = true);
    virtual void write(QJsonObject& json) const;

    virtual void setCylinderRadius(float r);
    float getCylinderRadius() const;

    virtual void setSphereRadius(float r);
    float getSphereRadius() const;

protected:
    float cylinderRadius;
    float sphereRadius;

    virtual QPair<int, int> remove();
    virtual void shift(QPair<int, int> offset);

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

#include "simulation/simulationlayer.h"

class Session;

class LayerItem : public TreeItem
{
public:
    explicit LayerItem(const QString& name, SimulationLayer* l, Session* s, TreeItem *parentItem = 0);
    ~LayerItem();

    void write(QJsonObject& json) const;

protected:
    QPair<int, int> remove();
    void shift(QPair<int, int> offset);

private:
    SimulationLayer* layer;
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

    void read(const QJsonObject& json, const MaterialListModel* mlm, Material* mat = Material::getDefault(), bool ve = true, bool vr = true);
    void write(QJsonObject& json) const;

    Camera* getCamera() const;

protected:
    QPair<int, int> remove();

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

class AtomItem : public TreeItem
{
public:
    explicit AtomItem(uint serial, const QByteArray& name, int offset, Session *s, TreeItem *parentItem = 0);
    ~AtomItem();

    void setLabel(const QString& l, const QRect& r);
    const QString& getLabel() const;

    void setCylinderRadius(float r);
    void setSphereRadius(float r);

    void setMaterial(const Material* material);
    void setFlag(VizFlag flag, bool on = true);

    QVector3D getPosition() const;

    void read(const QJsonObject& json, const MaterialListModel* mlm, Material* mat = Material::getDefault(), bool ve = true, bool vr = true);
    void write(QJsonObject& json) const;

protected:
    void shift(QPair<int, int> offset);

private:
    int id;
    QString label;
    Session *session;
};

class ChainItem : public TreeItem
{
public:
    explicit ChainItem(const QByteArray& chainID, Session* s, TreeItem *parentItem = 0);
    ~ChainItem();
};

class ResidueItem : public TreeItem
{
public:
    explicit ResidueItem(const QByteArray& resName, Session *s, TreeItem *parentItem = 0);
    ~ResidueItem();
};

#endif // TREEITEM_H
