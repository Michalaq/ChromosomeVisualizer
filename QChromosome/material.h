#ifndef MATERIAL_H
#define MATERIAL_H

#include <QWidget>
#include <QModelIndex>
#include <QUuid>
#include <QIcon>
#include <QProcess>
#include <QTextStream>

class Material : public QWidget
{
    Q_OBJECT
public:
    explicit Material(QString n = "Mat", QColor c = "#cbcbcb", float t = 0., QColor sc = Qt::white, float se = 10., QWidget *parent = 0);
    ~Material();

    QUuid getId() const;

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

    int getFinish() const;
    void setFinish(int f);

    static Material* getDefault();

    void paint(QPainter *painter, QRect bounds);

    void assign(const QPersistentModelIndex& ix, bool b = true);
    const QList<QPersistentModelIndex> &getAssigned() const;

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

    QTextStream &operator<<(QTextStream &stream) const;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    bool clicked;

    QUuid id;
    QString name;
    QColor color;
    float transparency;
    QColor specularColor;
    float specularExponent;
    int finish;

    static Material* dm;

    QList<QPersistentModelIndex> assignment;

    QIcon icon;
    void updateIcon();

    QIcon::Mode mode = QIcon::Normal;
    QMap<QWidget*, QRect> updates;
    QProcess p;


signals:

public slots:
};

QTextStream &operator<<(QTextStream &stream, const Material &mat);
QTextStream &operator<<(QTextStream &stream, const Material *mat);

#endif // MATERIAL_H
