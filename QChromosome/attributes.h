#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <QWidget>
#include <QModelIndexList>

namespace Ui {
class Attributes;
}

class Attributes : public QWidget
{
    Q_OBJECT

public:
    explicit Attributes(QWidget *parent = 0);
    ~Attributes();

    void setSelection(QModelIndexList& selectedRows, QModelIndexList& selectedAtoms);

private:
    Ui::Attributes *ui;

    QModelIndexList rows;
    QModelIndexList atoms;
};

#endif // ATTRIBUTES_H
