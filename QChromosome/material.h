#ifndef MATERIAL_H
#define MATERIAL_H

#include <QWidget>
#include <QModelIndex>

class Material : public QWidget
{
    Q_OBJECT
public:
    explicit Material(QString n = "Mat", QColor c = "#cbcbcb", float t = 0., QColor sc = Qt::white, float se = 10., QWidget *parent = 0);
    ~Material();

    QString getName() const;
    void setName(QString n);

    QColor getColor() const;
    void setColor(QColor c);

    float getTransparency() const;
    void setTransparency(float t);

    QColor getSpecularColor() const;
    void setSpecularColor(QColor c);

    float getSpecularExponent() const;
    void setSpecularExponent(qreal e);

    static Material* getDefault();

    void paint(QPainter *painter, QRect bounds);

    void assign(const QPersistentModelIndex& ix, bool b = true);
    QList<QPersistentModelIndex> getAssigned() const;

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

    std::ostream &operator<<(std::ostream &stream) const;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    bool clicked;

    QString name;
    QColor color;
    float transparency;
    QColor specularColor;
    float specularExponent;

    static Material* dm;

    QMap<QPersistentModelIndex, int> assignment;

signals:

public slots:
};

std::ostream &operator<<(std::ostream &stream, const Material &mat);

#endif // MATERIAL_H
