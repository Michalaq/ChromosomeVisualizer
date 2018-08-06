#ifndef ATOMATTRIBUTES_H
#define ATOMATTRIBUTES_H

#include <QStyledItemDelegate>

class QFontStyledDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit QFontStyledDelegate(QObject *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

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
    void updatePosition();

private:
    Ui::AtomAttributes *ui;

    TreeModel *model;
    QModelIndexList rows;
    QList<AtomItem*> atoms;

    void updateModelSelection();
};

#endif // ATOMATTRIBUTES_H
