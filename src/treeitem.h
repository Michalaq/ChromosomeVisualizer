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
    virtual QVariant data(int column, int role = Qt::DisplayRole) const;
    virtual bool setData(int column, const QVariant &value, int role = Qt::EditRole);
    int row() const;
    TreeItem *parentItem();
    void removeRows(int row, int count);

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

    QList<Material*> selectedTags() const;

protected:
    float cylinderRadius;
    float sphereRadius;

    virtual void remove();

private:
    QList<TreeItem*> m_childItems;
    QList<QVariant> m_itemData;
    TreeItem *m_parentItem;

    int selected_children_count = 0;
    int selected_tag_index = -1;
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
    explicit LayerItem(const QString& name, SimulationLayer* l, TreeItem *parentItem = 0);
    ~LayerItem();

    QVariant data(int column, int role = Qt::DisplayRole) const;

    void write(QJsonObject& json) const;

protected:
    void remove();

private:
    SimulationLayer* layer;

    static QVariant icon;
};

#include <QMatrix4x4>

enum Projection
{
    CP_Perspective,
    CP_Parallel
};

struct VizCameraInstance
{
    QMatrix4x4 projection   __attribute__((aligned(16)));
    QMatrix4x4 modelView    __attribute__((aligned(16)));
    VizFlags flags          __attribute__((aligned(4))) = VisibleInEditor | VisibleInRenderer;
    Projection ptype        __attribute__((aligned(4))) = CP_Perspective;
}                           __attribute__((aligned(16)));

class Camera;

class CameraItem : public TreeItem
{
public:
    explicit CameraItem(const QString& name, Camera* cam, Session* s, TreeItem *parentItem = 0);
    ~CameraItem();

    QVariant data(int column, int role = Qt::DisplayRole) const;
    bool setData(int column, const QVariant &value, int role = Qt::EditRole);

    QVector3D getPosition() const;
    void setPosition(const QVector3D& p);

    void setFlag(VizFlag flag, bool on = true);

    void read(const QJsonObject& json, const MaterialListModel* mlm, Material* mat = Material::getDefault(), bool ve = true, bool vr = true);
    void write(QJsonObject& json) const;

    Camera* getCamera() const;

protected:
    void remove();

private:
    Camera* camera;
    Session* session;

    static QVariant icon;

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
    explicit AtomItem(uint serial, const QByteArray& name, Session *s, TreeItem *parentItem = 0);
    ~AtomItem();

    QVariant data(int column, int role = Qt::DisplayRole) const;

    int getId() const;

    void setLabel(const QString& l, const QRect& r);
    const QString& getLabel() const;

    void setCylinderRadius(float r);
    void setSphereRadius(float r);

    void setMaterial(const Material* material);
    void setFlag(VizFlag flag, bool on = true);

    QVector3D getPosition() const;

    void read(const QJsonObject& json, const MaterialListModel* mlm, Material* mat = Material::getDefault(), bool ve = true, bool vr = true);
    void write(QJsonObject& json) const;

private:
    const int id;
    QString label;
    Session *session;

    static QVariant icon;
};

class ChainItem : public TreeItem
{
public:
    explicit ChainItem(const QByteArray& chainID, Session* s, TreeItem *parentItem = 0);
    ~ChainItem();

    QVariant data(int column, int role = Qt::DisplayRole) const;

private:
    static QVariant icon;
};

class ResidueItem : public TreeItem
{
public:
    explicit ResidueItem(const QByteArray& resName, Session *s, TreeItem *parentItem = 0);
    ~ResidueItem();

    QVariant data(int column, int role = Qt::DisplayRole) const;

private:
    static QVariant icon;
};

#endif // TREEITEM_H
