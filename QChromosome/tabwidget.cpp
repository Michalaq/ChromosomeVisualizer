#include "tabwidget.h"
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

        QProxyStyle::drawItemText(painter, _rect, Qt::AlignLeft | (flags & ~Qt::AlignHorizontal_Mask), pal, enabled, text, textRole);

        painter->restore();
    }
};

// conversion from different units to points
static const QMap<QString, qreal> unit2pt({{"cm", 28.3464567}, {"mm", 2.83464567}, {"Inches", 72}, {"Points", 1}, {"Picas", 12}, {"Pixels/cm", 28.3464567}, {"Pixels/Inch (DPI)", 72}});

// maximal output resolution
static const double dim_max = 16000;

#include <QStandardPaths>
#include "session.h"
#include "jpegdialog.h"
#include "targadialog.h"

TabWidget::TabWidget(Session* s, QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidget),
    session(s),
    jpegSettings(new JPEGDialog(this)),
    targaSettings(new TARGADialog(this)),
    defaultPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
{
    ui->setupUi(this);

    tabBar()->setStyle(new MyProxyStyle);

    // width
    connect(ui->doubleSpinBox_4, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double value) {
        if (ui->checkBox_5->isChecked())
            ui->doubleSpinBox_5->setValue(value / ui->doubleSpinBox_7->value(), false);
        else
        {
            ui->doubleSpinBox_7->setValue(value / ui->doubleSpinBox_5->value(), false);
            emit filmRatioChanged(ui->doubleSpinBox_7->value());
        }

        imageResolution.setWidth(qRound(value * ui->doubleSpinBox_6->value() * widthUnit / resolutionUnit));
        ui->label_40->setText(QString("%1 x %2 Pixel").arg(imageResolution.width()).arg(imageResolution.height()));
    });

    // height
    connect(ui->doubleSpinBox_5, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double value) {
        if (ui->checkBox_5->isChecked())
            ui->doubleSpinBox_4->setValue(value * ui->doubleSpinBox_7->value(), false);
        else
        {
            ui->doubleSpinBox_7->setValue(ui->doubleSpinBox_4->value() / value, false);
            emit filmRatioChanged(ui->doubleSpinBox_7->value());
        }

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

    // film aspect
    connect(ui->doubleSpinBox_7, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double value) {
        ui->doubleSpinBox_5->setValue(ui->doubleSpinBox_4->value() / value);
        emit filmRatioChanged(value);
    });

    // frame rate
    connect(ui->spinBox_2, QOverload<int>::of(&SpinBox::valueChanged), this, &TabWidget::updateFrames);

    // frame range
    connect(ui->comboBox_5, &ComboBox::currentTextChanged, [this](const QString& value) {
        if (value == "Current frame")
            setDocumentTime(session->projectSettings->getDocumentTime());

        if (value == "All frames")
            setFrameRange(session->projectSettings->getMinimumTime(), session->projectSettings->getMaximumTime());

        if (value == "Preview range")
            setFrameRange(session->projectSettings->getPreviewMinTime(), session->projectSettings->getPreviewMaxTime());
    });

    // from
    connect(ui->spinBox_3, QOverload<int>::of(&SpinBox::valueChanged), [this](int value) {
        ui->comboBox_5->setCurrentText("Manual");
        ui->spinBox_4->setMinimum(value);
        updateFrames();
    });

    // to
    connect(ui->spinBox_4, QOverload<int>::of(&SpinBox::valueChanged), [this](int value) {
        ui->comboBox_5->setCurrentText("Manual");
        ui->spinBox_3->setMaximum(value);
        updateFrames();
    });

    // step
    connect(ui->spinBox_5, QOverload<int>::of(&SpinBox::valueChanged), this, &TabWidget::updateFrames);

    // format
    connect(ui->comboBox, &QComboBox::currentTextChanged, [this](const QString& value) {
        ui->spinBox->setReadOnly(value != "PNG" && value != "PPM");
        ui->spinBox->setValue(8);

        ui->pushButton->hide();

        if (value == "JPEG")
        {
            disconnect(ui->pushButton);
            connect(ui->pushButton, &QPushButton::clicked, jpegSettings, &QDialog::open);
            ui->pushButton->show();
        }

        if (value == "TARGA")
        {
            disconnect(ui->pushButton);
            connect(ui->pushButton, &QPushButton::clicked, targaSettings, &QDialog::open);
            ui->pushButton->show();
        }
    });

    // antialias
    connect(ui->checkBox_6, &QCheckBox::clicked, [this](bool checked) {
        ui->widget->setEnabled(checked);
    });

    // jitter
    connect(ui->checkBox_7, &QCheckBox::clicked, [this](bool checked) {
        ui->widget_2->setEnabled(checked);
    });

    ui->comboBox_4->setCurrentText("Pixels/Inch (DPI)");
    ui->comboBox_3->setCurrentText("Pixels");
    ui->doubleSpinBox_4->setValue(320);
    ui->doubleSpinBox_5->setValue(240);
    ui->doubleSpinBox_6->setValue(72);
    ui->spinBox_2->setValue(1);
    ui->comboBox_5->setCurrentText("Current frame");
    ui->comboBox->setCurrentText("PNG");
    ui->label_12->setText(defaultPath);
    ui->label_23->setText(defaultPath);
}

TabWidget::~TabWidget()
{
    delete ui;
}

void TabWidget::setDocumentTime(int time)
{
    if (ui->comboBox_5->currentText() == "Current frame")
        setFrameRange(time, time);
}

void TabWidget::setMinimumTime(int time)
{
    if (ui->comboBox_5->currentText() == "All frames")
        setFrameRange(time, session->projectSettings->getMaximumTime());
}

void TabWidget::setMaximumTime(int time)
{
    if (ui->comboBox_5->currentText() == "All frames")
        setFrameRange(session->projectSettings->getMinimumTime(), time);
}

void TabWidget::setPreviewMinTime(int time)
{
    if (ui->comboBox_5->currentText() == "Preview range")
        setFrameRange(time, session->projectSettings->getPreviewMaxTime());
}

void TabWidget::setPreviewMaxTime(int time)
{
    if (ui->comboBox_5->currentText() == "Preview range")
        setFrameRange(session->projectSettings->getPreviewMinTime(), time);
}

void TabWidget::setLastFrame(int time)
{
    ui->spinBox_4->setMaximum(time);
}

#include "rendersettings.h"

void TabWidget::writeINIFile(QFile *ini) const
{
    QTextStream stream(ini);

    stream << "; This INI file was generated by QChromosome 4D Studio\n";

    stream << "\n; General output options\n";

    stream << "Width=" << imageResolution.width() << "\n";
    stream << "Height=" << imageResolution.height() * (RenderSettings::getInstance()->cam360() ? 2 : 1) << "\n";

    if (ui->spinBox_3->value() == ui->spinBox_4->value())
        stream << "Clock=" << ui->spinBox_3->value() << "\n";
    else
    {
        stream << "Initial_Frame=" << session->projectSettings->getMinimumTime() * ui->spinBox_2->value() << "\n";
        stream << "Final_Frame=" << session->projectSettings->getMaximumTime() * ui->spinBox_2->value() << "\n";

        stream << "Initial_Clock=" << session->projectSettings->getMinimumTime() << "\n";
        stream << "Final_Clock=" << session->projectSettings->getMaximumTime() << "\n";

        stream << "Subset_Start_Frame=" << startFrame << "\n";
        stream << "Subset_End_Frame=" << endFrame << "\n";
        stream << "Frame_Step=" << ui->spinBox_5->value() << "\n";
    }

    stream << "\n; File output options\n";

    switch (ui->comboBox->currentIndex())
    {
    case 0:
        stream << "Output_File_Type=B\n";
        break;
    case 1:
        switch (targaSettings->getCompression())
        {
        case 0:
            stream << "Output_File_Type=T\n";
            break;
        case 1:
            stream << "Output_File_Type=C\n";
            break;
        }
        break;
    case 2:
        stream << "Output_File_Type=E\n";
        break;
    case 3:
        stream << "Output_File_Type=H\n";
        break;
    case 4:
        stream << "Output_File_Type=J\n";
        stream << "Compression=" << qMax(2, jpegSettings->getQuality()) << "\n";
        break;
    case 5:
        stream << "Output_File_Type=N\n";
        break;
    case 6:
        stream << "Output_File_Type=P\n";
        break;
    }

    stream << "Bits_Per_Color=" << ui->spinBox->value() << "\n";

    stream << "Output_Alpha=" << (ui->checkBox_2->isChecked() ? "on" : "off") << "\n";

    stream << "\n; Tracing options\n";

    stream << "Quality=" << ui->spinBox_6->value() << "\n";

    if (ui->checkBox_6->isChecked())
    {
        stream << "Antialias=on\n";
        switch (ui->comboBox_6->currentIndex())
        {
        case 0:
            stream << "Sampling_Method=1\n";
            break;
        case 1:
            stream << "Sampling_Method=2\n";
            break;
        }
        stream << "Antialias_Threshold=" << ui->doubleSpinBox->value() << "\n";
        if (ui->checkBox_7->isChecked())
        {
            stream << "Jitter=on\n";
            stream << "Jitter_Amount=" << ui->doubleSpinBox_2->value() << "\n";
        }
        else
            stream << "Jitter=off\n";

        stream << "Antialias_Depth=" << ui->spinBox_7->value() << "\n";
    }
    else
        stream << "Antialias=off\n";
}

#include <QMetaMethod>

void TabWidget::connectNotify(const QMetaMethod &signal)
{
    if (signal == QMetaMethod::fromSignal(&TabWidget::filmRatioChanged))
        emit filmRatioChanged(ui->doubleSpinBox_7->value());
}

void TabWidget::setFrameRange(int min, int max)
{
    ui->spinBox_3->setMaximum(max, false);
    ui->spinBox_3->setValue(min, false);

    ui->spinBox_4->setMinimum(min, false);
    ui->spinBox_4->setValue(max, false);

    updateFrames();
}

void TabWidget::updateFrames()
{
    startFrame = ui->spinBox_3->value() * ui->spinBox_2->value();
    endFrame = ui->spinBox_4->value() * ui->spinBox_2->value();

    ui->label_48->setText(QString("%1 (from %2 to %3)").arg((endFrame - startFrame) / ui->spinBox_5->value() + 1).arg(startFrame).arg(endFrame));
}
