#ifndef BLIND_H
#define BLIND_H

#include <QWidget>

class Blind : public QWidget
{
    Q_OBJECT
public:
    explicit Blind(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);

private:
    qreal aspectRatio;

signals:

public slots:
    void setAspectRatio(qreal ar);
};

#endif // BLIND_H
