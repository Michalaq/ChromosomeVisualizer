#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QMainWindow>
#include <QMap>

namespace Ui
{
    class Preferences;
}

class TreeModel;

typedef void(TreeModel::*PointerToMemberFunction)(const QModelIndex&);

class Preferences : public QMainWindow
{
    Q_OBJECT
public:
    ~Preferences();

    static Preferences* getInstance();

    PointerToMemberFunction coloringMethod() const;

    QVariant materialAt(int i) const;

signals:
    void coloringMethodChanged(PointerToMemberFunction);

public slots:

private:
    explicit Preferences(QWidget *parent = 0);

    Ui::Preferences *ui;

    static Preferences* instance;
};

#endif // PREFERENCES_H
