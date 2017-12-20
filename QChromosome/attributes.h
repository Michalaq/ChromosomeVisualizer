#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <QWidget>

namespace Ui {
class Attributes;
}

class Attributes : public QWidget
{
    Q_OBJECT

public:
    explicit Attributes(QWidget *parent = 0);
    ~Attributes();

private:
    Ui::Attributes *ui;
};

#endif // ATTRIBUTES_H
