#include "rendersettings.h"
#include "ui_rendersettings.h"

RenderSettings* RenderSettings::instance = nullptr;

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

        QProxyStyle::drawItemText(painter, _rect, Qt::AlignLeft | (flags & ~Qt::AlignHorizontal_Mask), pal, enabled, text, textRole);

        painter->restore();
    }
};

// conversion from different units to points
static const QMap<QString, qreal> unit2pt({{"cm", 28.3464567}, {"mm", 2.83464567}, {"Inches", 72}, {"Points", 1}, {"Picas", 12}, {"Pixels/cm", 28.3464567}, {"Pixels/Inch (DPI)", 72}});

// maximal output resolution
static const double dim_max = 16000;

RenderSettings::RenderSettings(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RenderSettings)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    ui->TabWidget->tabBar()->setStyle(new MyProxyStyle);

    // width
    connect(ui->doubleSpinBox_4, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double value) {
        if (ui->checkBox_5->isChecked())
            ui->doubleSpinBox_5->setValue(value / aspectRatio, false);
        else
            emit aspectRatioChanged(aspectRatio = value / ui->doubleSpinBox_5->value());

        imageResolution.setWidth(qRound(value * ui->doubleSpinBox_6->value() * widthUnit / resolutionUnit));
        ui->label_40->setText(QString("%1 x %2 Pixel").arg(imageResolution.width()).arg(imageResolution.height()));
    });

    // height
    connect(ui->doubleSpinBox_5, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double value) {
        if (ui->checkBox_5->isChecked())
            ui->doubleSpinBox_4->setValue(value * aspectRatio, false);
        else
            emit aspectRatioChanged(aspectRatio = ui->doubleSpinBox_4->value() / value);

        imageResolution.setHeight(qRound(value * ui->doubleSpinBox_6->value() * widthUnit / resolutionUnit));
        ui->label_40->setText(QString("%1 x %2 Pixel").arg(imageResolution.width()).arg(imageResolution.height()));
    });

    // resolution
    connect(ui->doubleSpinBox_6, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double value) {
        resolution = value;

        if (ui->comboBox_3->currentText() == "Pixels")
            widthUnit = resolutionUnit / value;

        double multiplier = value * widthUnit / resolutionUnit;

        ui->doubleSpinBox_4->setMaximum(dim_max / multiplier);
        ui->doubleSpinBox_5->setMaximum(dim_max / multiplier);

        imageResolution = QSize(qRound(ui->doubleSpinBox_4->value() * multiplier), qRound(ui->doubleSpinBox_5->value() * multiplier));
        ui->label_40->setText(QString("%1 x %2 Pixel").arg(imageResolution.width()).arg(imageResolution.height()));
    });

    // width units
    connect(ui->comboBox_3, &ComboBox::currentTextChanged, [this](const QString& value) {
        widthUnit = value == "Pixels" ? resolutionUnit / ui->doubleSpinBox_6->value() : unit2pt[value];

        double multiplier = ui->doubleSpinBox_6->value() * widthUnit / resolutionUnit;

        ui->doubleSpinBox_4->setMaximum(dim_max / multiplier, false);
        ui->doubleSpinBox_5->setMaximum(dim_max / multiplier, false);

        ui->doubleSpinBox_4->setValue(imageResolution.width() / multiplier, false);
        ui->doubleSpinBox_5->setValue(imageResolution.height() / multiplier, false);
    });

    // resolution unit
    connect(ui->comboBox_4, &ComboBox::currentTextChanged, [this](const QString& value) {
        resolution /= resolutionUnit;
        resolutionUnit = unit2pt[value];
        resolution *= resolutionUnit;

        ui->doubleSpinBox_6->setValue(resolution, false);
    });

    ui->comboBox_4->setCurrentText("Pixels/Inch (DPI)");
    ui->comboBox_3->setCurrentText("Pixels");
    ui->doubleSpinBox_4->setValue(320);
    ui->doubleSpinBox_5->setValue(240);
    ui->doubleSpinBox_6->setValue(72);

    //TODO
    units = { {"px", 72}, {"cm", 2.54}, {"mm", 25.4}, {"in", 1}, {"pt", 72}, {"pc", 6} };

    aspectRatio = ui->doubleSpinBox->value() / ui->doubleSpinBox_2->value();
    currentUnit = ui->comboBox->currentText();
    currentResolutionUnit = ui->comboBox_2->currentIndex() ? "in" : "cm";

    updateOutputSize();

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

    connect(ui->cam360CheckBox, &QCheckBox::toggled, [this](bool c) {
        if (c) ui->checkBox_4->setChecked(false);
        ui->checkBox_4->setDisabled(c);
    });
    //TODO
}

RenderSettings::~RenderSettings()
{
    delete ui;
}

RenderSettings* RenderSettings::getInstance()
{
    return instance ? instance : instance = new RenderSettings();
}

//TODO
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

bool RenderSettings::openFile() const
{
    return ui->openFileCheckBox->isChecked();
}

int RenderSettings::framerate() const
{
    return ui->spinBox->value();
}

bool RenderSettings::overlays() const
{
    return ui->checkBox_4->isChecked();
}

bool RenderSettings::render() const
{
    return ui->checkBox_2->isChecked();
}

bool RenderSettings::exportPOV() const
{
    return ui->checkBox_3->isChecked();
}

QString RenderSettings::POVfileName() const
{
    return ui->lineEdit_3->text();
}
//TODO

#include <QMetaMethod>

void RenderSettings::connectNotify(const QMetaMethod &signal)
{
    if (signal == QMetaMethod::fromSignal(&RenderSettings::aspectRatioChanged))
        emit aspectRatioChanged(aspectRatio);
}

//TODO
void RenderSettings::updateOutputSize()
{
    const qreal multiplier = units["px"] / units[currentUnit];

    outSize = QSize(ui->doubleSpinBox->value() * multiplier + .5, ui->doubleSpinBox_2->value() * multiplier + .5);

    ui->label_7->setText(QString("%1 x %2 px").arg(QString::number(outSize.width()), QString::number(outSize.height())));
}
//TODO
