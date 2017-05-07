#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QWidget>

namespace Ui
{
    class Preferences;
}

class Material;

class Preferences : public QWidget
{
    Q_OBJECT
public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();

    void mousePressEvent(QMouseEvent *event);

signals:
    void materialsSelected(const QList<Material*>&);

public slots:

private:
    Ui::Preferences *ui;
};

#endif // PREFERENCES_H
