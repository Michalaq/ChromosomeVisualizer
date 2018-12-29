#ifndef LAYERATTRIBUTES_H
#define LAYERATTRIBUTES_H

#include "attributes.h"

namespace Ui {
class LayerAttributes;
}

class LayerItem;

class LayerAttributes : public MetaAttributes
{
    Q_OBJECT

public:
    explicit LayerAttributes(QWidget *parent = 0);
    ~LayerAttributes();

    void setSelection(TreeModel* selectedModel, const QModelIndexList& selectedRows);
    void unsetSelection();

public slots:


private:
    Ui::LayerAttributes *ui;

    TreeModel *model;
    QModelIndexList rows;
    QList<LayerItem*> layers;

    void updateModelSelection();
    void updatePosition();
};

#endif // LAYERATTRIBUTES_H
