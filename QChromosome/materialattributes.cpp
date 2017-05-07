#include "materialattributes.h"
#include "ui_materialattributes.h"
#include "material.h"

MaterialAttributes::MaterialAttributes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaterialAttributes)
{
    ui->setupUi(this);

    // set color
    ui->widget->showAlphaChannel(false);

    connect(ui->widget, &Picker::valueChanged, [this] (QColor color) {
        color.setAlpha(selection->getColor().alpha());
        selection->setColor(color);
        update();
    });

    // set transparency
    connect(ui->doubleSpinBox_2, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this] (double val) {
        auto color = selection->getColor();
        color.setAlphaF(1. - val / 100);
        selection->setColor(color);
        update();
    });

    // set specular width
    connect(ui->doubleSpinBox_3, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this] (double val) {
        selection->setSpecularExponent(val);
        update();
    });

    // set specular color
    ui->widget_2->showAlphaChannel(false);

    connect(ui->widget_2, &Picker::valueChanged, [this] (QColor color) {
        selection->setSpecularColor(color);
        update();
    });
}

MaterialAttributes::~MaterialAttributes()
{
    delete ui;
}

void MaterialAttributes::handleSelection(const QList<Material*> &selected)
{
    if (selected.isEmpty())
        return hide();

    selection = selected.first();

    // set title
    title = "Material ";

    ui->label_14->setText(title + "[Mat]");

    // set color
    ui->widget->setValue(selection->getColor().rgb(), false);

    // set transparency
    ui->doubleSpinBox_2->setValue((1. - selection->getColor().alphaF()) * 100, false);

    // set specular width
    ui->doubleSpinBox_3->setValue(selection->getSpecularExponent(), false);

    // set specular color
    ui->widget_2->setValue(selection->getSpecularColor(), false);

    update();
    show();
}

#include <QPainter>

void MaterialAttributes::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    auto bounds = ui->widget_3->geometry();

    QPainter painter(this);

    painter.save();

    painter.setClipRect(bounds);
    painter.fillRect(bounds, "#999999");

    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(bounds.center());
    painter.rotate(-45.);

    int size = std::min(bounds.width(), bounds.height());
    qreal step = (qreal)size / 5 / sqrt(2);

    for (int i = -2; i < 3; i++)
        painter.fillRect(QRectF(-size, 2 * i * step, 2 * size, step), "#666666");

    painter.restore();

    selection->paint(&painter, bounds);
}
