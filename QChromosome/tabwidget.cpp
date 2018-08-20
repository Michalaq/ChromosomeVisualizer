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

#include "session.h"

TabWidget::TabWidget(Session* s, QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidget),
    session(s)
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
    });

    // to
    connect(ui->spinBox_4, QOverload<int>::of(&SpinBox::valueChanged), [this](int value) {
        ui->comboBox_5->setCurrentText("Manual");
        ui->spinBox_3->setMaximum(value);
    });

    ui->comboBox_4->setCurrentText("Pixels/Inch (DPI)");
    ui->comboBox_3->setCurrentText("Pixels");
    ui->doubleSpinBox_4->setValue(320);
    ui->doubleSpinBox_5->setValue(240);
    ui->doubleSpinBox_6->setValue(72);
    ui->spinBox_2->setValue(30);
    ui->comboBox_5->setCurrentText("Current frame");
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
}
