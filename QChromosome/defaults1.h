#ifndef DEFAULTS1_H
#define DEFAULTS1_H

#include <QWidget>
#include <QVariant>

namespace Ui
{
    class Defaults1;
}

//TODO should be implemented as singleton
class Defaults1 : public QWidget
{
    Q_OBJECT
public:
    explicit Defaults1(QWidget *parent = 0);
    ~Defaults1();

    static const QVariant &chainnumber2color(int cn);

signals:

public slots:

private:
    Ui::Defaults1 *ui;

    static QMap<int, QVariant> cn2defaults; // maps chain numbers to their defaults
};

#endif // DEFAULTS1_H
