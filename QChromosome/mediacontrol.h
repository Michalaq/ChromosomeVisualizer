#ifndef MEDIACONTROL_H
#define MEDIACONTROL_H

#include <QPushButton>
#include <QGraphicsColorizeEffect>

class MediaControl : public QPushButton
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    explicit MediaControl(QWidget *parent = 0);
    virtual ~MediaControl();

    QColor color() const;
    void setColor(const QColor &c);

private:
    QGraphicsColorizeEffect *effect;
};

#endif // MEDIACONTROL_H
