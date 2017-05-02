#ifndef MATERIAL_H
#define MATERIAL_H

#include <QWidget>

class MaterialRenderer;

class Material : public QWidget
{
    Q_OBJECT
public:
    explicit Material(QWidget *parent = 0);

    QColor getColor() const;
    void setColor(QColor c);

    void paint(QPainter *painter, QRect bounds);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    bool clicked;

    QColor color;
    //...

    MaterialRenderer *renderer;

signals:

public slots:
};

#endif // MATERIAL_H
