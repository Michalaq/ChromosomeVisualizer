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

    static int bt2typename(int bt);
    static int ev2typename(std::vector<int> ev);
    static int rs2typename(std::string rs);

    static const QString &typename2label(int tn);
    static const QVariant &typename2color(int tn);

signals:

public slots:

private:
    Ui::Defaults *ui;

    static QMap<int, int> bt2tn; // maps binder type to its typename
    static QMap<std::vector<int>, int> ev2tn; // maps energy vector to bead typename
    static QMap<std::string, int> rs2tn; // maps residue name to typename
    static QMap<int, QPair<QString, QVariant>> tn2defaults; // maps typenames to their defaults

    static int typenames;
};

#endif // DEFAULTS_H
