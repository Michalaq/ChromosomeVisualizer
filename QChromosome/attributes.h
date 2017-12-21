#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <QWidget>
#include <QModelIndexList>

namespace Ui {
class Attributes;
}

class TreeModel;

class Attributes : public QWidget
{
    Q_OBJECT

public:
    explicit Attributes(QWidget *parent = 0);
    ~Attributes();

    void setSelection(TreeModel* selectedModel, QModelIndexList& selectedRows, QModelIndexList& selectedAtoms);

private:
    Ui::Attributes *ui;

    TreeModel *model;
    QModelIndexList rows;
    QModelIndexList atoms;

    void updateTitle();
};

#endif // ATTRIBUTES_H
