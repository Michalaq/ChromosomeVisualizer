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
#include <QSettings>

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

    //PROJECT SETTINGS
    connect(ui->toolButton_2, &QToolButton::clicked, [this] {
        QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui->lineEdit_2->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (!path.isEmpty())
        {
            ui->lineEdit_2->setText(path);

            QSettings settings;
            settings.setValue("povraypath", path);
        }
    });

    connect(ui->lineEdit_2, &QLineEdit::textChanged, [this](const QString& path) {
        QSettings settings;
        settings.setValue("povraypath", path);
    });

    QSettings settings;
    ui->lineEdit_2->setText(settings.value("povraypath", "/usr/local/share/povray-3.7").toString());
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

QString RenderSettings::quality() const
{
    return ui->qualitySpinBox->text();
}

bool RenderSettings::antiAliasing() const
{
    return ui->aaCheckBox->isChecked();
}

QString RenderSettings::aaSamplingMethod() const
{
    return ui->aaSamplingSpinBox->text();
}

QString RenderSettings::aaThreshold() const
{
    return ui->aaThresholdDoubleSpinBox->text();
}

bool RenderSettings::aaJitter() const
{
    return ui->aaJitterCheckBox->isChecked();
}

QString RenderSettings::aaJitterAmount() const
{
    return ui->aaJitterAmountDoubleSpinBox->text();
}

QString RenderSettings::aaDepth() const
{
    return ui->aaDepthSpinBox->text();
}

bool RenderSettings::timestamp() const
{
    return ui->timestampCheckBox->isChecked();
}

QString RenderSettings::ambient() const
{
    return ui->ambientDoubleSpinBox->text();
}

QString RenderSettings::diffuse() const
{
    return ui->diffuseDoubleSpinBox->text();
}

QString RenderSettings::phong() const
{
    return ui->phongDoubleSpinBox->text();
}

QString RenderSettings::phongSize() const
{
    return ui->phongSizeSpinBox->text();
}

QString RenderSettings::metallic() const
{
    return ui->metallicSpinBox->text();
}

QString RenderSettings::iridescence() const
{
    return ui->iridDoubleSpinBox->text();
}

QString RenderSettings::iridescenceThickness() const
{
    return ui->iridThicknessDoubleSpinBox->text();
}

QString RenderSettings::iridescenceTurbulence() const
{
    return ui->iridTurbulenceDoubleSpinBox->text();
}

bool RenderSettings::cam360() const
{
    return ui->cam360CheckBox->isChecked();
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

#include "ui_rswidget.h"

RenderSettingsWidget::RenderSettingsWidget(RenderSettings *rs, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RenderSettingsWidget)
{
    ui->setupUi(this);

    layout()->addWidget(rs);
}

RenderSettingsWidget::~RenderSettingsWidget()
{
    delete ui;
}
