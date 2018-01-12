#ifndef ATOMATTRIBUTES_H
#define ATOMATTRIBUTES_H

#include <QWidget>

namespace Ui {
class AtomAttributes;
}

class AtomAttributes : public QWidget
{
    Q_OBJECT

public:
    explicit AtomAttributes(QWidget *parent = 0);
    ~AtomAttributes();

private:
    Ui::AtomAttributes *ui;
};

#endif // ATOMATTRIBUTES_H
