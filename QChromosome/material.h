#ifndef MATERIAL_H
#define MATERIAL_H

#include <QWidget>

class Material : public QWidget
{
    Q_OBJECT
public:
    explicit Material(QColor c = "#cbcbcb", QColor sc = Qt::white, float se = 20., QWidget *parent = 0);

    QColor getColor() const;
    void setColor(QColor c);

    QColor getSpecularColor() const;
    void setSpecularColor(QColor c);

    float getSpecularExponent() const;
    void setSpecularExponent(qreal e);

    static Material* getDefault();

    void paint(QPainter *painter, QRect bounds);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    bool clicked;

    QColor color;
    QColor specularColor;
    float specularExponent;

    static Material* dm;

signals:

public slots:
};

#endif // MATERIAL_H
