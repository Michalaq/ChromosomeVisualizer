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

    static const char* bt2name(int bt);
    static const char* ev2name(std::vector<int> ev);

signals:

public slots:

private:
    Ui::Defaults *ui;

    static QMap<int, const char*> bt2n; // maps binder type to its name
    static QMap<std::vector<int>, const char*> ev2n; // maps energy vector to bead name

    QVariant previous;
    int key1;
    std::vector<int> key2;

    QVector<QByteArray> dump;
};

#endif // DEFAULTS_H
