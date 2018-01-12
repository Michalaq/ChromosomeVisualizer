#ifndef ATOMATTRIBUTES_H
#define ATOMATTRIBUTES_H

#include "attributes.h"

namespace Ui {
class AtomAttributes;
}

class AtomAttributes : public MetaAttributes
{
    Q_OBJECT

public:
    explicit AtomAttributes(QWidget *parent = 0);
    ~AtomAttributes();

    void setSelection(TreeModel* selectedModel, const QModelIndexList& selectedRows);
    void unsetSelection();

public slots:


private:
    Ui::AtomAttributes *ui;

    TreeModel *model;
    QModelIndexList rows;

    void updateModelSelection();
    void updatePosition();
};

#endif // ATOMATTRIBUTES_H
