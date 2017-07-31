#include "material.h"
#include "materialrenderer.h"

Material* Material::dm = nullptr;

Material::Material(QString n, QColor c, float t, QColor sc, float se, QWidget *parent) :
    QWidget(parent),
    clicked(false),
    name(n),
    color(c),
    transparency(t),
    specularColor(sc),
    specularExponent(se)
{
    setFixedSize(45, 45);
}

QString Material::getName() const
{
    return name;
}

void Material::setName(QString n)
{
    name = n;
}

QColor Material::getColor() const
{
    return color;
}

void Material::setColor(QColor c)
{
    color = c;
    update();
}

float Material::getTransparency() const
{
    return transparency;
}

void Material::setTransparency(float t)
{
    transparency = t;
    update();
}

QColor Material::getSpecularColor() const
{
    return specularColor;
}

void Material::setSpecularColor(QColor c)
{
    specularColor = c;
    update();
}

float Material::getSpecularExponent() const
{
    return specularExponent;
}

void Material::setSpecularExponent(qreal e)
{
    specularExponent = e;
    update();
}

Material* Material::getDefault()
{
    return dm ? dm : (dm = new Material);
}

void Material::paint(QPainter *painter, QRect bounds)
{
    if (!(bounds.width() > 0 && bounds.height() > 0))
        return;

    int size = std::min(bounds.width(), bounds.height());

    QRect b(0, 0, size, size);
    b.moveCenter(bounds.center());

    MaterialRenderer::getInstance()->paint(painter, b, this);
}

void Material::assign(const QPersistentModelIndex &ix, bool b)
{
    auto it = assignment.find(ix);

    if (b)
    {
        if (it != assignment.end())
            it.value()++;
        else
            assignment.insert(ix, 1);
    }
    else
    {
        if (--it.value() == 0)
            assignment.erase(it);
    }
}

QList<QPersistentModelIndex> Material::getAssigned() const
{
    return assignment.keys();
}

void Material::read(const QJsonObject& json)
{
    name = json["Name"].toString();

    const QJsonObject color_ = json["Color"].toObject();
    color = color_["Color"].toString();
    transparency = color_["Transparency"].toDouble();

    const QJsonObject specular = json["Specular"].toObject();
    specularExponent = specular["Shininess exponent"].toDouble();
    specularColor = specular["Specular color"].toString();
}

void Material::write(QJsonObject& json) const
{
    json["Name"] = name;

    QJsonObject color_;
    color_["Color"] = color.name();
    color_["Transparency"] = transparency;
    json["Color"] = color_;

    QJsonObject specular;
    specular["Shininess exponent"] = specularExponent;
    specular["Specular color"] = specularColor.name();
    json["Specular"] = specular;
}

void Material::mousePressEvent(QMouseEvent *event)
{
    clicked = true;

    QWidget::mousePressEvent(event);
}

#include <QMimeData>
#include <QDrag>

void Material::mouseMoveEvent(QMouseEvent *event)
{
    if (clicked)
    {
        clicked = false;

        QDrag *drag = new QDrag(this);
        drag->setMimeData(new QMimeData);
        drag->exec(Qt::CopyAction | Qt::MoveAction);
    }
    else
        QWidget::mouseMoveEvent(event);
}

#include <QPainter>

void Material::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter p(this);
    paint(&p, rect());
}
