#include "materialbrowser.h"
#include "ui_materialbrowser.h"
#include "material.h"
#include "session.h"

MaterialBrowser* MaterialBrowser::instance = nullptr;

MaterialBrowser::MaterialBrowser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MaterialBrowser)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    Material::getDefault();
    mat[0] = new Material("White", "#ffffff", .5);
    mat[1] = new Material("Maroon", "#800000");
    mat[2] = new Material("Red", "#ff0000");
    mat[3] = new Material("Olive", "#808000");
    mat[4] = new Material("Yellow", "#ffff00");
    mat[5] = new Material("Green", "#008000");
    mat[6] = new Material("Lime", "#00ff00");
    mat[7] = new Material("Teal", "#008080");
    mat[8] = new Material("Aqua", "#00ffff");
    mat[9] = new Material("Navy", "#000080");
    mat[10] = new Material("Blue", "#0000ff");
    mat[11] = new Material("Purple", "#800080");
    mat[12] = new Material("Fuchsia", "#ff00ff");

    connect(ui->actionNew_Material, &QAction::triggered, [this]() {
        auto m = qobject_cast<MaterialListModel*>(listView->model());
        m->prepend(new Material(m->next_name()));
        listView->setCurrentIndex(m->index(0));
    });

    connect(ui->actionClose_Manager, SIGNAL(triggered(bool)), this, SLOT(close()));
}

MaterialBrowser::~MaterialBrowser()
{
    delete ui;
    qDeleteAll(mat, mat + 13);
}

MaterialBrowser* MaterialBrowser::getInstance()
{
    return instance ? instance : instance = new MaterialBrowser();
}

ListView* MaterialBrowser::makeListView()
{
    ListView* lv = new ListView;

    auto* mlm = new MaterialListModel;
    lv->setModel(mlm);

    connect(lv->selectionModel(), &QItemSelectionModel::currentChanged, [this](const QModelIndex& index, const QModelIndex&) {
        if (index.isValid())
            emit materialsSelected({index.data(Qt::DecorationRole).value<Material*>()});
        else
            emit materialsSelected({});
        update();
    });

    ui->stackedWidget->addWidget(lv);

    return lv;
}

void MaterialBrowser::setSession(Session *s)
{
    ui->stackedWidget->setCurrentWidget(listView = s->listView);
}

#include <QPainter>

void MaterialBrowser::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter p(this);

    if (listView->selectionModel()->hasSelection())
        listView->selectionModel()->currentIndex().data(Qt::DecorationRole).value<Material*>()->paint(&p, ui->widget_3->geometry().translated(ui->splitter->pos() + ui->centralwidget->pos()).marginsRemoved(QMargins(10, 10, 10, 10)));
}
