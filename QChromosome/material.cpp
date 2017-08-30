#include "material.h"
#include "materialrenderer.h"

Material* Material::dm = nullptr;

Material::Material(QString n, QColor c, float t, QColor sc, float se, QWidget *parent) :
    QWidget(parent),
    clicked(false),
    id(QUuid::createUuid()),
    name(n),
    color(c),
    transparency(t),
    specularColor(sc),
    specularExponent(se)
{
    setFixedSize(45, 45);
}

Material::~Material()
{

}

QUuid Material::getId() const
{
    return id;
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
    if (b)
        assignment.append(ix);
    else
        assignment.removeOne(ix);
}

const QList<QPersistentModelIndex>& Material::getAssigned() const
{
    return assignment;
}

#include <QJsonObject>

void Material::read(const QJsonObject& json)
{
    id = json["UUID"].toString();
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
    json["UUID"] = id.toString();
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

#include "rendersettings.h"

std::ostream &Material::operator<<(std::ostream &stream) const
{
    auto renderSettings = RenderSettings::getInstance();

    stream << "#declare m" << QString::number(quintptr(this), 16).toStdString() << " =\n"
           << "texture {\n"
           << " pigment {\n"
           << "  color rgb<" << color.redF() << ", " << color.greenF() << ", " << color.blueF() << ">\n"
           << "  transmit " << 1. - (1. - transparency) * (1. - transparency) << "\n"
           << " }\n"
           << " finish {\n"
           << "  ambient " << renderSettings->ambient().toStdString() << "\n"
           << "  diffuse " << renderSettings->diffuse().toStdString() << "\n"
           << "  phong " << renderSettings->phong().toStdString() << "\n"
           << "  phong_size " << renderSettings->phongSize().toStdString() << "\n"
           << "  metallic " << renderSettings->metallic().toStdString() << "\n"
           << "  irid { " << renderSettings->iridescence().toStdString() << "\n"
           << "   thickness " << renderSettings->iridescenceThickness().toStdString() << "\n"
           << "   turbulence " << renderSettings->iridescenceTurbulence().toStdString() << "\n"
           << "  }\n"
           << "  specular 1.0\n"
           << "  roughness 0.02\n"
           << " }\n"
           << "}\n";

    return stream;
}

std::ostream &operator<<(std::ostream &stream, const Material &mat)
{
    return mat << stream;
}
