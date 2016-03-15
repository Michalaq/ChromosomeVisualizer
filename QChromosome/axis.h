#ifndef AXIS_H
#define AXIS_H

#include <QWidget>
#include <QMatrix4x4>

class Axis : public QWidget
{
    Q_OBJECT
public:
    explicit Axis(QWidget *parent = 0);

signals:

public slots:
    void setModelView(const QMatrix4x4& mat);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QMatrix4x4 modelView;
};

#endif // AXIS_H
