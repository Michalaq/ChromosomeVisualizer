#ifndef MATERIAL_H
#define MATERIAL_H

#include <QWidget>
#include <QModelIndex>
#include <QUuid>
#include <QIcon>
#include <QProcess>
#include <QTextStream>
#include "glbuffer.h"

struct material_data_t
{
    QRgb color              __attribute__((aligned(4)));
    QRgb specularColor      __attribute__((aligned(4)));
    float specularExponent  __attribute__((aligned(4)));
}                           __attribute__((aligned(16)));

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

    double getAmbient() const;
    void setAmbient(double a);

    double getDiffuse() const;
    void setDiffuse(double d);

    double getPhong() const;
    void setPhong(double p);

    int getPhongSize() const;
    void setPhongSize(int p);

    bool getMetallic() const;
    void setMetallic(bool m);

    double getIridescence() const;
    void setIridescence(double i);

    double getIridescenceThickness() const;
    void setIridescenceThickness(double i);

    double getIridescenceTurbulence() const;
    void setIridescenceTurbulence(double i);

    static Material* getDefault();

    void paint(QPainter *painter, QRect bounds);

    void assign(const QPersistentModelIndex& ix, bool b = true);
    const QList<QPersistentModelIndex> &getAssigned() const;

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

    QTextStream &operator<<(QTextStream &stream) const;

    int getIndex() const;

    static const Material* fetch(int index);
    static void writePOVMaterials(QTextStream &stream);

    static OpenGLArraysBuffer<material_data_t>& getBuffer();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    bool clicked;

    QUuid id;
    QString name;
    QColor color;
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

    int index;
    static OpenGLArraysBuffer<material_data_t> buffer;
    static QVector<const Material*> library;

    double ambient;
    double diffuse;
    double phong;
    int phongSize;
    bool metallic;
    double iridescence;
    double iridescenceThickness;
    double iridescenceTurbulence;
};

QTextStream &operator<<(QTextStream &stream, const Material &mat);
QTextStream &operator<<(QTextStream &stream, const Material *mat);

#endif // MATERIAL_H
