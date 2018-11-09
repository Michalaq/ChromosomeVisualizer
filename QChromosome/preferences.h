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

    const QVariant& resName2color(const QString& resName) const;
    const QVariant& chainID2color(uint chainID) const;

signals:
    void coloringMethodChanged(PointerToMemberFunction);

public slots:

private:
    explicit Preferences(QWidget *parent = 0);

    Ui::Preferences *ui;

    static Preferences* instance;

    QMap<QString, QVariant> rs2defaults; // maps residue names to their defaults
    QMap<uint, QVariant> cn2defaults; // maps chain numbers to their defaults
};

#endif // PREFERENCES_H
