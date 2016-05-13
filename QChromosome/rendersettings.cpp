#include "rendersettings.h"
#include "ui_rendersettings.h"

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

    aspectRatio = ui->doubleSpinBox->value() / ui->doubleSpinBox_2->value();
    currentUnit = ui->comboBox->currentText();
    currentResolutionUnit = ui->comboBox_2->currentIndex() ? "in" : "cm";

    updateOutputSize();

    connect(ui->toolButton, &QToolButton::clicked, [this] {
        QString path = QFileDialog::getSaveFileName(this, tr("Save File"), ui->lineEdit->text(), tr("Images (*.png *.xpm *.jpg)"));

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

        updateOutputSize();
    });

    connect(ui->doubleSpinBox_2, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value) {
        if (ui->checkBox->isChecked())
            ui->doubleSpinBox->setValue(value * aspectRatio);
        else
        {
            aspectRatio = ui->doubleSpinBox->value() / ui->doubleSpinBox_2->value();
            emit aspectRatioChanged(aspectRatio);
        }

        updateOutputSize();
    });

    connect(ui->doubleSpinBox_3, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value) {
        value *= units[currentResolutionUnit];

        units["px"] =  value;

        updateOutputSize();
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

        updateOutputSize();
    });
}

RenderSettings::~RenderSettings()
{
    delete ui;
}

QSize RenderSettings::outputSize() const
{
    return outSize;
}

QString RenderSettings::saveFile() const
{
    return ui->lineEdit->text();
}

#include <QMetaMethod>

void RenderSettings::connectNotify(const QMetaMethod &signal)
{
    if (signal == QMetaMethod::fromSignal(&RenderSettings::aspectRatioChanged))
        emit aspectRatioChanged(aspectRatio);
}

void RenderSettings::updateOutputSize()
{
    const qreal multiplier = units["px"] / units[currentUnit];

    outSize = QSize(ui->doubleSpinBox->value() * multiplier + .5, ui->doubleSpinBox_2->value() * multiplier + .5);

    ui->label_7->setText(QString("%1 x %2 px").arg(QString::number(outSize.width()), QString::number(outSize.height())));
}