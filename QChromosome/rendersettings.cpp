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

RenderSettings::RenderSettings(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidget)
{
    ui->setupUi(this);
    tabBar()->setStyle(new MyProxyStyle);

    connect(ui->toolButton, &QToolButton::clicked, [this] {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), ui->lineEdit->text(), tr("Images (*.png *.xpm *.jpg)"));

        if (!fileName.isEmpty())
            ui->lineEdit->setText(fileName);
    });
}

RenderSettings::~RenderSettings()
{
    delete ui;
}

double RenderSettings::getOutputWidth() const
{
    return ui->doubleSpinBox->value();
}

double RenderSettings::getOutputHeight() const
{
    return ui->doubleSpinBox_2->value();
}

QString RenderSettings::getFile() const
{
    return ui->lineEdit->text();
}
