#include "preferences.h"
#include "ui_preferences.h"

Preferences* Preferences::instance = nullptr;

#include "myproxystyle.h"
#include "materialbrowser.h"

Preferences::Preferences(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    ui->TabWidget->tabBar()->setStyle(new MyProxyStyle);

    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this] {
        emit coloringMethodChanged(coloringMethod());
    });

    auto model = new MaterialListModel(this);

    for (int i = 12; i > 0; i--)
        model->prepend(MaterialBrowser::getInstance()->mat[i]);

    ui->listView->setItemDelegate(new MaterialDelegate);
    ui->listView->setModel(model);
}

Preferences::~Preferences()
{
    delete ui;
}

Preferences* Preferences::getInstance()
{
    return instance ? instance : instance = new Preferences();
}

#include "treemodel.h"

PointerToMemberFunction Preferences::coloringMethod() const
{
    switch (ui->comboBox->currentIndex())
    {
    case 0:
        return &TreeModel::colorByResidue;
    case 1:
        return &TreeModel::colorByChain;
    }

    Q_UNREACHABLE();
}

QVariant Preferences::materialAt(int i) const
{
    auto model = ui->listView->model();
    return model->index(i % model->rowCount(), 0).data(Qt::DecorationRole);
}
