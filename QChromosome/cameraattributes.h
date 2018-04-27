#ifndef CAMERAATTRIBUTES_H
#define CAMERAATTRIBUTES_H

#include "attributes.h"
#include <QModelIndexList>

namespace Ui {
class CameraAttributes;
}

class TreeModel;
class Camera;

class CameraAttributes : public MetaAttributes
{
    Q_OBJECT

public:
    explicit CameraAttributes(QWidget *parent = 0);
    ~CameraAttributes();

    void setSelection(TreeModel* selectedModel, const QModelIndexList& selectedRows);
    void unsetSelection();

signals:
    void selected(const QPersistentModelIndex&);

public slots:


private:
    Ui::CameraAttributes *ui;

    TreeModel *model;
    QModelIndexList rows;
    QList<Camera*> cameras;

    void updateModelSelection();
    void updateModelView();
    void updateProjection();
};

#endif // CAMERAATTRIBUTES_H
