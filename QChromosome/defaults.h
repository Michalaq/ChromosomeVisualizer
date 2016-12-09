#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <QWidget>
#include <QVariant>

namespace Ui
{
    class Defaults;
}

//TODO should be implemented as singleton
class Defaults : public QWidget
{
    Q_OBJECT
public:
    explicit Defaults(QWidget *parent = 0);
    ~Defaults();

    static std::string bt2name(int bt);
    static std::string ev2name(std::vector<int> ev);

signals:

public slots:

private:
    Ui::Defaults *ui;

    static QMap<int, std::string> bt2n; // maps binder type to its name
    static QMap<std::vector<int>, std::string> ev2n; // maps energy vector to bead name

    QVariant previous;
    int key;
};

#endif // DEFAULTS_H
