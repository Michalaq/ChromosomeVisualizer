#include "preferences.h"
#include "ui_preferences.h"

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

#include <QSettings>
#include <QStandardPaths>
#include "flowlayout.h"
#include "material.h"

Preferences::Preferences(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);

    ui->TabWidget->tabBar()->setStyle(new MyProxyStyle);

    /*TODO do usunięcia, kiedy znajdę lepsze miejsce*/

    connect(ui->lineEdit, &QLineEdit::editingFinished, [this]() {
        QSettings().setValue("povraypath", ui->lineEdit->text());
    });

    connect(ui->lineEdit_2, &QLineEdit::editingFinished, [this]() {
        QSettings().setValue("locallib", ui->lineEdit_2->text());
    });

    QSettings settings;
    ui->lineEdit->setText(settings.value("povraypath", "/usr/local/share/povray-3.7").toString());
    ui->lineEdit_2->setText(settings.value("locallib", QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString());

    ui->lineEdit_2->setType(FileEdit::FE_Directory);

    FlowLayout *flowLayout = new FlowLayout;
    flowLayout->addWidget(new Material("", Qt::red));
    flowLayout->addWidget(new Material("", Qt::green));
    flowLayout->addWidget(new Material("", Qt::blue));

    ui->tab_2->setLayout(flowLayout);
}

Preferences::~Preferences()
{
    delete ui;
}

#include <QMouseEvent>

void Preferences::mousePressEvent(QMouseEvent *event)
{
    auto* m = qobject_cast<Material*>(childAt(event->pos()));

    if (m)
        emit materialsSelected(QList<Material*>({m}));
    else
        if (childAt(event->pos())->objectName() == "tab_2")
            emit materialsSelected(QList<Material*>());
}
