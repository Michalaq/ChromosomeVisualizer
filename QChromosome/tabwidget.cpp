#include "tabwidget.h"
#include "ui_tabwidget.h"

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

    // width
    connect(ui->doubleSpinBox_4, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double value) {
        if (ui->checkBox_5->isChecked())
        {
            ui->doubleSpinBox_5->setValue(value / aspectRatio, false);
            imageResolution.setHeight(qRound(ui->doubleSpinBox_5->value() * ui->doubleSpinBox_6->value() * widthUnit / resolutionUnit));
        }
        else
        {
            ui->doubleSpinBox_7->setValue(aspectRatio = value / ui->doubleSpinBox_5->value(), false);
            emit filmRatioChanged(aspectRatio);
        }

        imageResolution.setWidth(qRound(value * ui->doubleSpinBox_6->value() * widthUnit / resolutionUnit));
        ui->label_40->setText(QString("%1 x %2 Pixel").arg(imageResolution.width()).arg(imageResolution.height()));
    });

    // height
    connect(ui->doubleSpinBox_5, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double value) {
        if (ui->checkBox_5->isChecked())
        {
            ui->doubleSpinBox_4->setValue(value * aspectRatio, false);
            imageResolution.setWidth(qRound(ui->doubleSpinBox_4->value() * ui->doubleSpinBox_6->value() * widthUnit / resolutionUnit));
        }
        else
        {
            ui->doubleSpinBox_7->setValue(aspectRatio = ui->doubleSpinBox_4->value() / value, false);
            emit filmRatioChanged(aspectRatio);
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
        aspectRatio = value;
        ui->doubleSpinBox_5->setValue(ui->doubleSpinBox_4->value() / value);
        emit filmRatioChanged(aspectRatio);
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

    // save (image)
    connect(ui->checkBox, &QCheckBox::toggled, ui->widget_3, &QWidget::setEnabled);

    // format
    connect(ui->comboBox, &QComboBox::currentTextChanged, [this](const QString& value) {
        ui->spinBox->setReadOnly(value != "PNG" && value != "PPM");
        ui->spinBox->setValue(8);

        ui->pushButton->hide();

        if (value == "JPEG")
        {
            ui->pushButton->disconnect();
            connect(ui->pushButton, &QPushButton::clicked, jpegSettings, &QDialog::open);
            ui->pushButton->show();
        }

        if (value == "TARGA")
        {
            ui->pushButton->disconnect();
            connect(ui->pushButton, &QPushButton::clicked, targaSettings, &QDialog::open);
            ui->pushButton->show();
        }

        switch (ui->comboBox->currentIndex())
        {
        case 0:
            ext = "bmp";
            break;
        case 1:
            ext = "tga";
            break;
        case 2:
            ext = "exr";
            break;
        case 3:
            ext = "hdr";
            break;
        case 4:
            ext = "jpg";
            break;
        case 5:
            ext = "png";
            break;
        case 6:
            ext = "ppm";
            break;
        }
    });

    // save (translator)
    connect(ui->checkBox_3, &QCheckBox::toggled, ui->widget_4, &QWidget::setEnabled);

    // antialias
    connect(ui->checkBox_6, &QCheckBox::toggled, ui->widget, &QWidget::setEnabled);

    // jitter
    connect(ui->checkBox_7, &QCheckBox::toggled, ui->widget_2, &QWidget::setEnabled);

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

QTextStream& TabWidget::operator<<(QTextStream& stream) const
{
    stream << "; This INI file was generated by QChromosome 4D Studio\n";

    stream << "\n; General output options\n";

    stream << "Width=" << imageResolution.width() << "\n";
    stream << "Height=" << imageResolution.height() * (session->currentCamera->getMode() == Camera::CM_Symmetrical ? 2 : 1) << "\n";

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
        case 2:
            stream << "Sampling_Method=3\n";
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

    return stream;
}

bool TabWidget::openFile() const
{
    return ui->checkBox_8->isChecked();
}

bool TabWidget::render() const
{
    return ui->checkBox_9->isChecked();
}

bool TabWidget::saveOutput() const
{
    return ui->checkBox->isChecked();
}

bool TabWidget::saveTraslator() const
{
    return ui->checkBox_3->isChecked();
}

QDir TabWidget::getOutputDir() const
{
    return QFileInfo(defaultPath, ui->lineEdit->text()).dir();
}

QDir TabWidget::getTranslatorDir() const
{
    return QFileInfo(defaultPath, ui->lineEdit_2->text()).dir();
}

QString TabWidget::getOutputName() const
{
    return QFileInfo(ui->lineEdit->text()).completeBaseName();
}

QString TabWidget::getTranslatorName() const
{
    return QFileInfo(ui->lineEdit_2->text()).completeBaseName();
}

QPair<int, int> TabWidget::frameRange() const
{
    return {ui->spinBox_3->value(), ui->spinBox_4->value()};
}

QString TabWidget::getExtension() const
{
    return ext;
}

QStringList& operator<<(QStringList& list, int n)
{
    return list << QString::number(n);
}

QString operator+(const QString& string, int n)
{
    return string + QString::number(n);
}

void TabWidget::getFFmpegArgs(QStringList& argv) const
{
    const QString& name = "scene";

    argv << "-y"
         << "-start_number" << startFrame
         << "-framerate" << session->projectSettings->getFPS() * ui->spinBox_2->value()
         << "-i" << name + "%0" + QString::number(session->projectSettings->getMaximumTime() * ui->spinBox_2->value()).length() + "d." + ext;

    if (ui->checkBox_4->isChecked())
        argv << "-vf" << QString("[in]drawtext=text='%{pts\\:hms\\:%1}': x=10: y=10: fontcolor=white, drawtext=text='%{eif\\:n+1\\:d}/%2 (%{eif\\:n+%3\\:d} F)': x=10: y=15+lh: fontcolor=white[out]").arg(1. * ui->spinBox_3->value() / session->projectSettings->getFPS()).arg(endFrame - startFrame + 1).arg(startFrame);

    argv << name + ".avi";
}

void TabWidget::read(const QJsonObject& json)
{
    const QJsonObject general = json["General"].toObject();

    const QJsonObject output = json["General output options"].toObject();
    ui->comboBox_3->setCurrentText(output["Width unit"].toString());
    ui->doubleSpinBox_4->setValue(output["Width"].toDouble());
    ui->doubleSpinBox_5->setValue(output["Height"].toDouble());
    ui->checkBox_5->setChecked(output["Lock ratio"].toBool());
    ui->comboBox_4->setCurrentText(output["Resolution unit"].toString());
    ui->doubleSpinBox_6->setValue(output["Resolution"].toDouble());
    ui->spinBox_2->setValue(output["Frame rate"].toInt());
    ui->comboBox_5->setCurrentText(output["Frame range"].toString());
    ui->spinBox_3->setValue(output["From"].toInt());
    ui->spinBox_4->setValue(output["To"].toInt());
    ui->spinBox_5->setValue(output["Frame step"].toInt());
    ui->textEdit->setPlainText(output["Annotations"].toString());

    const QJsonObject file = json["File output options"].toObject();

    const QJsonObject imageFileOutput = file["Image file output"].toObject();
    ui->checkBox->setChecked(imageFileOutput["Save"].toBool());
    ui->lineEdit->setText(imageFileOutput["File"].toString());
    ui->comboBox->setCurrentText(imageFileOutput["Format"].toString());
    ui->spinBox->setValue(imageFileOutput["Depth"].toInt());
    ui->checkBox_2->setChecked(imageFileOutput["Alpha channel"].toBool());

    if (imageFileOutput["Format"] == "JPEG")
        jpegSettings->read(imageFileOutput);

    if (imageFileOutput["Format"] == "TARGA")
        targaSettings->read(imageFileOutput);

    const QJsonObject translator = file["Translator"].toObject();
    ui->checkBox_3->setChecked(translator["Save"].toBool());
    ui->lineEdit_2->setText(translator["File"].toString());
    ui->comboBox_7->setCurrentText(translator["Target application"].toString());

    const QJsonObject tracing = json["Tracing"].toObject();

    const QJsonObject qualitySettings = tracing["Quality settings"].toObject();
    ui->spinBox_6->setValue(qualitySettings["Quality"].toInt());

    const QJsonObject antialiasing = tracing["Anti-Aliasing options"].toObject();
    ui->checkBox_6->setChecked(antialiasing["Antialias"].toBool());
    ui->comboBox_6->setCurrentText(antialiasing["Sampling method"].toString());
    ui->doubleSpinBox->setValue(antialiasing["Threshold"].toDouble());
    ui->checkBox_7->setChecked(antialiasing["Jitter"].toBool());
    ui->doubleSpinBox_2->setValue(antialiasing["Jitter amount"].toDouble());
    ui->spinBox_7->setValue(antialiasing["Depth"].toInt());
}

void TabWidget::write(QJsonObject& json) const
{
    QJsonObject general;
    json["General"] = general;

    QJsonObject output;
    output["Width unit"] = ui->comboBox_3->currentText();
    output["Width"] = ui->doubleSpinBox_4->value();
    output["Height"] = ui->doubleSpinBox_5->value();
    output["Lock ratio"] = ui->checkBox_5->isChecked();
    output["Resolution unit"] = ui->comboBox_4->currentText();
    output["Resolution"] = ui->doubleSpinBox_6->value();
    output["Frame rate"] = ui->spinBox_2->value();
    output["Frame range"] = ui->comboBox_5->currentText();
    output["From"] = ui->spinBox_3->value();
    output["To"] = ui->spinBox_4->value();
    output["Frame step"] = ui->spinBox_5->value();
    output["Annotations"] = ui->textEdit->toPlainText();
    json["General output options"] = output;

    QJsonObject file;

    QJsonObject imageFileOutput;
    imageFileOutput["Save"] = ui->checkBox->isChecked();
    imageFileOutput["File"] = ui->lineEdit->text();
    imageFileOutput["Format"] = ui->comboBox->currentText();
    imageFileOutput["Depth"] = ui->spinBox->value();
    imageFileOutput["Alpha channel"] = ui->checkBox_2->isChecked();

    if (imageFileOutput["Format"] == "JPEG")
        jpegSettings->write(imageFileOutput);

    if (imageFileOutput["Format"] == "TARGA")
        targaSettings->write(imageFileOutput);

    file["Image file output"] = imageFileOutput;

    QJsonObject translator;
    translator["Save"] = ui->checkBox_3->isChecked();
    translator["File"] = ui->lineEdit_2->text();
    translator["Target application"] = ui->comboBox_7->currentText();
    file["Translator"] = translator;

    json["File output options"] = file;

    QJsonObject tracing;

    QJsonObject qualitySettings;
    qualitySettings["Quality"] = ui->spinBox_6->value();
    tracing["Quality settings"] = qualitySettings;

    QJsonObject antialiasing;
    antialiasing["Antialias"] = ui->checkBox_6->isChecked();
    antialiasing["Sampling method"] = ui->comboBox_6->currentText();
    antialiasing["Threshold"] = ui->doubleSpinBox->value();
    antialiasing["Jitter"] = ui->checkBox_7->isChecked();
    antialiasing["Jitter amount"] = ui->doubleSpinBox_2->value();
    antialiasing["Depth"] = ui->spinBox_7->value();
    tracing["Anti-Aliasing options"] = antialiasing;

    json["Tracing"] = tracing;
}

#include <QMetaMethod>

void TabWidget::connectNotify(const QMetaMethod &signal)
{
    if (signal == QMetaMethod::fromSignal(&TabWidget::filmRatioChanged))
        emit filmRatioChanged(aspectRatio);
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
