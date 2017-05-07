#include "materialattributes.h"
#include "ui_materialattributes.h"

MaterialAttributes::MaterialAttributes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaterialAttributes)
{
    ui->setupUi(this);
}

MaterialAttributes::~MaterialAttributes()
{
    delete ui;
}

#include "material.h"

void MaterialAttributes::handleSelection(const QList<Material*> &selected)
{
    if (selected.isEmpty())
        return hide();

    selection = selected;

    //

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

    selection.first()->paint(&painter, bounds);
}
