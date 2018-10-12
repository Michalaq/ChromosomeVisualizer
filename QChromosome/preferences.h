#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QWidget>

namespace Ui
{
    class Preferences;
}

class TreeModel;

typedef void(TreeModel::*PointerToMemberFunction)(const QModelIndex&);

class Preferences : public QWidget
{
    Q_OBJECT
public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();

    PointerToMemberFunction coloringMethod() const;

    static int bt2typename(int bt);
    static int ev2typename(std::vector<int> ev);
    static int rs2typename(std::string rs);

    static const QString &typename2label(int tn);
    static const QVariant &typename2color(int tn);
    static const QVariant &chainnumber2color(int cn);

public slots:

private:
    Ui::Preferences *ui;

    static QMap<int, int> bt2tn; // maps binder type to its typename
    static QMap<std::vector<int>, int> ev2tn; // maps energy vector to bead typename
    static QMap<std::string, int> rs2tn; // maps residue name to typename
    static QMap<int, QPair<QString, QVariant>> tn2defaults; // maps typenames to their defaults
    static QMap<int, QVariant> cn2defaults; // maps chain numbers to their defaults

    static int typenames;
};

#endif // PREFERENCES_H
