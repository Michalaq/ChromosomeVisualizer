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

    uint bt2typename(int bt);
    uint ev2typename(std::vector<int> ev);
    uint rs2typename(const QString& rs);

    const QString &typename2label(uint tn);
    const QVariant &typename2color(uint tn);
    const QVariant &chainnumber2color(int cn);

public slots:

private:
    explicit Preferences(QWidget *parent = 0);

    Ui::Preferences *ui;

    static Preferences* instance;

    QMap<int, uint> bt2tn; // maps binder type to its typename
    QMap<std::vector<int>, uint> ev2tn; // maps energy vector to bead typename
    QMap<uint, QString> tn2label; // maps typenames to their labels
    QMap<uint, QVariant> tn2defaults; // maps typenames to their colors
    QMap<uint, QVariant> cn2defaults; // maps chain numbers to their defaults
};

#endif // PREFERENCES_H
