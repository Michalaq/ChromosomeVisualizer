#include "rendersettings.h"
#include "ui_tabwidget.h"

#include <QProxyStyle>
#include <QPainter>

class MyProxyStyle : public QProxyStyle
{
public:
    void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled, const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const
    {
        QRect _rect(QPoint(), rect.size().transposed());

        painter->save();

        painter->translate(rect.center());
        painter->rotate(90);
        painter->translate(-_rect.center());

        QProxyStyle::drawItemText(painter, _rect, flags, pal, enabled, text, textRole);

        painter->restore();
    }
};

#include <QFileDialog>
#include <QtMath>

RenderSettings::RenderSettings(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidget)
{
    ui->setupUi(this);

    tabBar()->setStyle(new MyProxyStyle);

    units = { {"px", 72}, {"cm", 2.54}, {"mm", 25.4}, {"in", 1}, {"pt", 72}, {"pc", 6} };

    aspectRatio = 320. / 240.;
    currentUnit = "px";
    currentResolutionUnit = "in";

    connect(ui->toolButton, &QToolButton::clicked, [this] {
        QString path = QFileDialog::getSaveFileName(this, tr("File"), ui->lineEdit->text(), tr("Images (*.png *.xpm *.jpg)"));

        if (!path.isEmpty())
            ui->lineEdit->setText(path);
    });

    connect(ui->doubleSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value) {
        if (ui->checkBox->isChecked())
            ui->doubleSpinBox_2->setValue(value / aspectRatio);
        else
        {
            aspectRatio = ui->doubleSpinBox->value() / ui->doubleSpinBox_2->value();
            emit aspectRatioChanged(aspectRatio);
        }

        const qreal multiplier = units["px"] / units[currentUnit];

        QString width = QString::number(qFloor(ui->doubleSpinBox->value() * multiplier + .5));
        QString height = QString::number(qFloor(ui->doubleSpinBox_2->value() * multiplier + .5));

        ui->label_7->setText(QString("%1 x %2 px").arg(width, height));
    });

    connect(ui->doubleSpinBox_2, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value) {
        if (ui->checkBox->isChecked())
            ui->doubleSpinBox->setValue(value * aspectRatio);
        else
        {
            aspectRatio = ui->doubleSpinBox->value() / ui->doubleSpinBox_2->value();
            emit aspectRatioChanged(aspectRatio);
        }

        const qreal multiplier = units["px"] / units[currentUnit];

        QString width = QString::number(qFloor(ui->doubleSpinBox->value() * multiplier + .5));
        QString height = QString::number(qFloor(ui->doubleSpinBox_2->value() * multiplier + .5));

        ui->label_7->setText(QString("%1 x %2 px").arg(width, height));
    });

    connect(ui->doubleSpinBox_3, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value) {
        value *= units[currentResolutionUnit];

        units["px"] =  value;

        const qreal multiplier = value / units[currentUnit];

        QString width = QString::number(qFloor(ui->doubleSpinBox->value() * multiplier + .5));
        QString height = QString::number(qFloor(ui->doubleSpinBox_2->value() * multiplier + .5));

        ui->label_7->setText(QString("%1 x %2 px").arg(width, height));
    });

    connect(ui->comboBox, &QComboBox::currentTextChanged, [this](const QString& text) {
        const qreal multiplier = units[text] / units[currentUnit];

        currentUnit = text;

        ui->doubleSpinBox->setValue(ui->doubleSpinBox->value() * multiplier);
        ui->doubleSpinBox_2->setValue(ui->doubleSpinBox_2->value() * multiplier);
    });

    connect(ui->comboBox_2, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index) {
        qreal value = ui->doubleSpinBox_3->value() * units[currentResolutionUnit];

        currentResolutionUnit = (index ? "in" : "cm");

        ui->doubleSpinBox_3->setValue(value / units[currentResolutionUnit]);

        const qreal multiplier = value / units[currentUnit];

        QString width = QString::number(qFloor(ui->doubleSpinBox->value() * multiplier + .5));
        QString height = QString::number(qFloor(ui->doubleSpinBox_2->value() * multiplier + .5));

        ui->label_7->setText(QString("%1 x %2 px").arg(width, height));
    });
}

RenderSettings::~RenderSettings()
{
    delete ui;
}

QSize RenderSettings::outputSize() const
{
    const qreal multiplier = units["px"] / units[currentUnit];

    int width = qFloor(ui->doubleSpinBox->value() * multiplier + .5);
    int height = qFloor(ui->doubleSpinBox_2->value() * multiplier + .5);

    return QSize(width, height);
}

QString RenderSettings::saveFile() const
{
    return ui->lineEdit->text();
}
