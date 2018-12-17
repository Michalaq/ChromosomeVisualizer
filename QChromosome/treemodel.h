#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include "treeitem.h"

class Material;
class Simulation;
class Session;

enum VisibilityMode
{
    Editor = 3,
    Renderer = 4
};

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(Session *s, QObject *parent = 0);
    ~TreeModel() Q_DECL_OVERRIDE;

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

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

    void colorByResidue(const QModelIndex &root);
    void colorByChain(const QModelIndex &root);

    void addCamera(Camera* camera);
    void changeCamera(Camera* camera);
    void setCurrentCamera(QModelIndex index);

    void setMaterial(const QModelIndex& root, Material* m, int position = INT_MAX);
    Material* removeMaterial(const QModelIndex& root, int position);

    void setVisibility(const QModelIndex& index, Visibility v, VisibilityMode m);
    void setVisibility(const QModelIndexList& indices, Visibility v, VisibilityMode m);

    void setName(const QModelIndex& index, const QString& name);
    void setName(const QModelIndexList& indices, const QString& name);

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

    void prepend(TreeItem* item);

private:
    TreeItem *header;
    QPersistentModelIndex currentCamera;
    Session *session;
    QSet<Material*> materials;
    QMap<Camera*, QPersistentModelIndex> cameras;

    QString next_name() const;

    void propagateVisibility(const QModelIndex &root, VisibilityMode m, bool v);

    void dumpModel1(const QModelIndex& root, QVector<QPersistentModelIndex>& id);
    void dumpModel2(const QModelIndex& root, Material* m);
    void dumpModel3(const QModelIndex& root, Material* m);

signals:
    void propertyChanged();
    void attributeChanged();
    void cameraChanged(Camera*);
    void tagSelected(const QList<Material*>&);
};

#endif // TREEMODEL_H
