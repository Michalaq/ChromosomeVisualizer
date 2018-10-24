#include "material.h"

Material* Material::dm = nullptr;
GLBuffer<material_data_t> Material::buffer;
QVector<const Material*> Material::library;

Material::Material(QString n, QColor c, float t, QColor sc, float se, QWidget *parent) :
    QWidget(parent),
    clicked(false),
    id(QUuid::createUuid()),
    name(n),
    color(c),
    specularColor(sc),
    specularExponent(se),
    finish(0),
    ambient(.1),
    diffuse(.6),
    phong(.0),
    phongSize(40),
    metallic(false),
    iridescence(.0),
    iridescenceThickness(.0),
    iridescenceTurbulence(.0)
{
    color.setAlphaF(1. - t);

    setFixedSize(45, 45);
    updateIcon();

    index = buffer.count();
    buffer.append({color.rgba(), specularColor.rgba(), specularExponent});

    library.push_back(this);
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
    c.setAlphaF(color.alphaF());

    color = c;
    updateIcon();

    buffer[index].color = color.rgba();
}

float Material::getTransparency() const
{
    return 1. - color.alphaF();
}

void Material::setTransparency(float t)
{
    color.setAlphaF(1. - t);
    updateIcon();

    buffer[index].color = color.rgba();
}

QColor Material::getSpecularColor() const
{
    return specularColor;
}

void Material::setSpecularColor(QColor c)
{
    specularColor = c;
    updateIcon();

    buffer[index].specularColor = specularColor.rgba();
}

float Material::getSpecularExponent() const
{
    return specularExponent;
}

void Material::setSpecularExponent(qreal e)
{
    specularExponent = e;
    updateIcon();

    buffer[index].specularExponent = e;
}

int Material::getFinish() const
{
    return finish;
}

void Material::setFinish(int f)
{
    finish = f;
    updateIcon();
}

double Material::getAmbient() const
{
    return ambient;
}

void Material::setAmbient(double a)
{
    ambient = a;
    updateIcon();
}

double Material::getDiffuse() const
{
    return diffuse;
}

void Material::setDiffuse(double d)
{
    diffuse = d;
    updateIcon();
}

double Material::getPhong() const
{
    return phong;
}

void Material::setPhong(double p)
{
    phong = p;
    updateIcon();
}

int Material::getPhongSize() const
{
    return phongSize;
}

void Material::setPhongSize(int p)
{
    phongSize = p;
    updateIcon();
}

bool Material::getMetallic() const
{
    return metallic;
}

void Material::setMetallic(bool m)
{
    metallic = m;
    updateIcon();
}

double Material::getIridescence() const
{
    return iridescence;
}

void Material::setIridescence(double i)
{
    iridescence = i;
    updateIcon();
}

double Material::getIridescenceThickness() const
{
    return iridescenceThickness;
}

void Material::setIridescenceThickness(double i)
{
    iridescenceThickness = i;
    updateIcon();
}

double Material::getIridescenceTurbulence() const
{
    return iridescenceTurbulence;
}

void Material::setIridescenceTurbulence(double i)
{
    iridescenceTurbulence = i;
    updateIcon();
}

Material* Material::getDefault()
{
    return dm ? dm : (dm = new Material);
}

#include <QPainter>

void Material::paint(QPainter *painter, QRect bounds)
{
    if (p.state() != QProcess::NotRunning)
        updates[dynamic_cast<QWidget*>(painter->device())] = bounds;

    icon.paint(painter, bounds, Qt::AlignCenter, mode);
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
    color.setAlphaF(1. - color_["Transparency"].toDouble());

    const QJsonObject specular = json["Specular"].toObject();
    specularExponent = specular["Shininess exponent"].toDouble();
    specularColor = specular["Specular color"].toString();

    updateIcon();

    buffer[index] = {color.rgba(), specularColor.rgba(), specularExponent};
}

void Material::write(QJsonObject& json) const
{
    json["UUID"] = id.toString();
    json["Name"] = name;

    QJsonObject color_;
    color_["Color"] = color.name();
    color_["Transparency"] = 1. - color.alphaF();
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

void Material::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter p(this);
    paint(&p, rect());
}

#include <QTemporaryFile>

void Material::updateIcon()
{
    p.disconnect();
    p.close();

    mode = QIcon::Disabled;

    QTemporaryFile* f = new QTemporaryFile;
    f->open();

    QTextStream file(f);

    file << *this
         << "camera { perspective location <0,0,-2.8125> direction <0,0,1> right x up y }\n"
         << "sphere { <0,0,0>, 1 material { " << this << " } }\n"
         << "difference { box{ <-4,-4,-4>, <4,4,4> } union { box { <-5,-1,-5>, <5,5,1.4> } box{ <-5,1,-5>, <5,5,3.4> } cylinder { <-5,1,1.4>, <5,1,1.4>, 2 } } pigment { checker rgb <0.9,0.9,0.9> rgb <0.6,0.6,0.6> scale 0.5 } scale <1,1,0.75> }\n"
         << "light_source { <-3,4,-5> rgb <1,1,1> parallel area_light <5, 0, 0>, <0, 0, 5>, 5, 5 adaptive 1 jitter }\n";

    QStringList argv;
    argv << "+W256"
         << "+H256"
         << "+A"
         << "-GA"
         << "-D"
         << "-O-"
         << "+I\"" + f->fileName() + "\"";

    p.start("povray", argv);

    connect(&p, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [f, this](int exitCode){
        if (exitCode == 0)
        {
            QImage img;
            img.loadFromData(p.readAllStandardOutput(), "PNG");

            icon = QIcon(QPixmap::fromImage(img));
            mode = QIcon::Normal;

            for (auto i = updates.begin(); i != updates.end(); i++)
                if (i.key()) i.key()->update(i.value());

            updates.clear();
        }

        f->deleteLater();
    });
}

#include "rendersettings.h"

QTextStream &Material::operator<<(QTextStream &stream) const
{
    switch (finish)
    {
    case 0://custom
        stream << "#declare " << this << "tex =\n"
               << "texture {\n"
               << " pigment {\n"
               << "  color rgb<" << color.redF() << ", " << color.greenF() << ", " << color.blueF() << ">\n"
               << "  transmit " << 1. - color.alphaF() * color.alphaF() << "\n"
               << " }\n"
               << " finish {\n"
               << "  ambient " << ambient << "\n"
               << "  diffuse " << diffuse << "\n"
               << "  phong " << phong << "\n"
               << "  phong_size " << phongSize << "\n"
               << (metallic ? "  metallic\n" : "")
               << "  irid { " << iridescence << "\n"
               << "   thickness " << iridescenceThickness << "\n"
               << "   turbulence " << iridescenceTurbulence << "\n"
               << "  }\n"
               << "  specular 1.0\n"
               << "  roughness 0.02\n"
               << " }\n"
               << "}\n"
               << "#declare " << this << " = material {texture {" << this << "tex}}\n";
        break;
    case 1://metal
        stream << "#declare Metal" << this << " =\n"
               << "finish { metallic ambient 0.2 diffuse 0.7 brilliance 6 reflection 0.25 phong 0.75 phong_size 80 }\n"
               << "#declare " << this << "tex = texture { pigment{ rgbf <" << color.redF() << ", " << color.greenF() << ", " << color.blueF() << ", " << 1. - color.alphaF() * color.alphaF() << ">} finish{ Metal" << this << " }}\n"
               << "#declare " << this << " = material {texture {" << this << "tex}}\n";
        break;
    case 2://glass
        stream << "#declare Glass_Interior" << this << " = interior {ior 1.5}\n"
               << "#declare " << this << "tex = \n"
               << "texture {\n"
               << "pigment { rgbf <" << color.redF() << ", " << color.greenF() << ", " << color.blueF() << ", " << 1. - color.alphaF() * color.alphaF() << "> }\n"
               << "finish  { ambient 0.1 diffuse 0.1 reflection 0.1 specular 0.8 roughness 0.0003 phong 1 phong_size 400}\n"
               << "}\n"
               << "#declare " << this << " = material {texture {" << this << "tex} interior {Glass_Interior" << this << "}}\n";
        break;
    }

    return stream;
}

QTextStream &operator<<(QTextStream &stream, const Material &mat)
{
    return mat << stream;
}

QTextStream &operator<<(QTextStream &stream, const Material *mat)
{
    return stream << "m" << QString::number(quintptr(mat), 16);
}


int Material::getIndex() const
{
    return index;
}

const Material* Material::fetch(int index)
{
    return library[index];
}

void Material::writePOVMaterials(QTextStream &stream)
{
    for (auto material : library)
        if (!material->assignment.isEmpty())
            stream << *material;
}

GLBuffer<material_data_t> &Material::getBuffer()
{
    return buffer;
}
