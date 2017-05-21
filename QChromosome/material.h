#ifndef MATERIAL_H
#define MATERIAL_H

#include <QWidget>
#include <QModelIndex>

class Material : public QWidget
{
    Q_OBJECT
public:
    explicit Material(QColor c = "#cbcbcb", QColor sc = Qt::white, float se = 10., QWidget *parent = 0);

    QColor getColor() const;
    void setColor(QColor c);

    QColor getSpecularColor() const;
    void setSpecularColor(QColor c);

    float getSpecularExponent() const;
    void setSpecularExponent(qreal e);

    static Material* getDefault();

    void paint(QPainter *painter, QRect bounds);

    void assign(const QModelIndex& ix, bool b = true);
    QModelIndexList getAssigned() const;

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

    QMap<QModelIndex, int> assignment;

signals:

public slots:
};

#endif // MATERIAL_H
