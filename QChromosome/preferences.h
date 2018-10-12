#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QWidget>
#include <QMap>

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
    ~Preferences();

    static Preferences* getInstance();

    PointerToMemberFunction coloringMethod() const;

    int bt2typename(int bt);
    int ev2typename(std::vector<int> ev);
    int rs2typename(std::string rs);

    const QString &typename2label(int tn);
    const QVariant &typename2color(int tn);
    const QVariant &chainnumber2color(int cn);

public slots:

private:
    explicit Preferences(QWidget *parent = 0);

    Ui::Preferences *ui;

    static Preferences* instance;

    QMap<int, int> bt2tn; // maps binder type to its typename
    QMap<std::vector<int>, int> ev2tn; // maps energy vector to bead typename
    QMap<std::string, int> rs2tn; // maps residue name to typename
    QMap<int, QPair<QString, QVariant>> tn2defaults; // maps typenames to their defaults
    QMap<int, QVariant> cn2defaults; // maps chain numbers to their defaults

    int typenames;
};

#endif // PREFERENCES_H
