#ifndef SERIESATTRIBUTES_H
#define SERIESATTRIBUTES_H

#include "attributes.h"

namespace Ui {
class SeriesAttributes;
}

class SeriesItem;

class SeriesAttributes : public MetaAttributes
{
    Q_OBJECT

public:
    explicit SeriesAttributes(QWidget *parent = nullptr);
    ~SeriesAttributes();

    void setSelection(TreeModel* selectedModel, const QModelIndexList& selectedRows);
    void unsetSelection();

private:
    Ui::SeriesAttributes *ui;

    TreeModel *model;
    QModelIndexList rows;
    QList<SeriesItem*> series;

    void updateModelSelection();
};

#endif // SERIESATTRIBUTES_H
