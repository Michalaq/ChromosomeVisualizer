#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QWidget>

namespace Ui {
    class Viewport;
}

class Viewport : public QWidget
{
    Q_OBJECT
public:
    explicit Viewport(QWidget *parent = 0);
    ~Viewport();

signals:

public slots:

private:
    Ui::Viewport *ui;
};

#endif // VIEWPORT_H
