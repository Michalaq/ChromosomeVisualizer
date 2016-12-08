#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <QWidget>

namespace Ui
{
    class Defaults;
}

class Defaults : public QWidget
{
    Q_OBJECT
public:
    explicit Defaults(QWidget *parent = 0);
    ~Defaults();

signals:

public slots:

private:
    Ui::Defaults *ui;
};

#endif // DEFAULTS_H
