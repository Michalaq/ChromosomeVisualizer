#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <QWidget>
#include <QModelIndexList>

namespace Ui {
class Attributes;
}

class TreeModel;
class TreeItem;

class MetaAttributes : public QWidget
{
    Q_OBJECT

public:
    explicit MetaAttributes(QWidget *parent = 0);
    virtual ~MetaAttributes();

    virtual void setSelection(TreeModel* selectedModel, const QModelIndexList& selectedRows) = 0;
    virtual void unsetSelection() = 0;
};

class Attributes : public MetaAttributes
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
    QList<TreeItem*> items;

    void updateModelSelection();
    void updatePosition();
};

#endif // ATTRIBUTES_H
