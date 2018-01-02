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

    void setSelection(TreeModel* selectedModel, const QModelIndexList& selectedRows);
    void unsetSelection();

public slots:


private:
    Ui::Attributes *ui;

    TreeModel *model;
    QModelIndexList rows;

    void updateSelection();
};

#endif // ATTRIBUTES_H
