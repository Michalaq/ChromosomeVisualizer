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

public slots:

private:
    Ui::Preferences *ui;
};

#endif // PREFERENCES_H
