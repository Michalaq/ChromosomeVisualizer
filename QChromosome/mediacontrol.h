#ifndef MEDIACONTROL_H
#define MEDIACONTROL_H

#include <QPushButton>
#include <QGraphicsColorizeEffect>

class MediaControl : public QPushButton
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(QColor hoverColor READ hoverColor WRITE setHoverColor)

public:
    explicit MediaControl(QWidget *parent = 0);
    virtual ~MediaControl();

    QColor color() const;
    void setColor(const QColor &c);

    QColor hoverColor() const;
    void setHoverColor(const QColor &c);
    /* cf. QTBUG-2982 */

    void click();

protected:
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

    void paintEvent(QPaintEvent *event);

private:
    QGraphicsColorizeEffect *effect;

    QColor __color;
    QColor __hoverColor;
};

#endif // MEDIACONTROL_H
