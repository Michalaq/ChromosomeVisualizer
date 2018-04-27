#ifndef ATOMATTRIBUTES_H
#define ATOMATTRIBUTES_H

#include "attributes.h"

namespace Ui {
class AtomAttributes;
}

class AtomItem;

class AtomAttributes : public MetaAttributes
{
    Q_OBJECT

public:
    explicit AtomAttributes(QWidget *parent = 0);
    ~AtomAttributes();

    void setSelection(TreeModel* selectedModel, const QModelIndexList& selectedRows);
    void unsetSelection();

signals:
    void attributeChanged();

public slots:


private:
    Ui::AtomAttributes *ui;

    TreeModel *model;
    QModelIndexList rows;
    QList<AtomItem*> atoms;

    void updateModelSelection();
    void updatePosition();
};

#endif // ATOMATTRIBUTES_H
