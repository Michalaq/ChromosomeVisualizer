#include "preferences.h"
#include "ui_preferences.h"

Preferences* Preferences::instance = nullptr;

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

#include "materialbrowser.h"
#include "tagsdelegate.h"
#include "tablemodel.h"

Preferences::Preferences(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    ui->TabWidget->tabBar()->setStyle(new MyProxyStyle);

    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        ui->stackedWidget->setCurrentIndex(index);
        emit coloringMethodChanged(coloringMethod());
    });

   QVariantList mat[13];

    for (int i = 0; i < 13; i++)
        mat[i] = {QVariant::fromValue(MaterialBrowser::getInstance()->mat[i])};

    // third table
    ui->tableView_3->setItemDelegateForColumn(0, new TableNameDelegate(this));
    ui->tableView_3->setItemDelegateForColumn(1, new TagsDelegate(this));

    auto *m3 = new TableModel({"Residue Name", "Default Tags"}, this);

    ui->tableView_3->setModel(m3);

    connect(m3, &TableModel::foo, [this](QModelIndex ix, QVariant old) {
        QString pk;
        switch (ix.column())
        {
        case 0: // residue name
            pk = old.toString();

            if (!pk.isEmpty())
                rs2defaults.remove(pk);

            rs2defaults.insert(ix.data().toString(), ix.sibling(ix.row(), 1).data());

            break;
        case 1: // atom color
            pk = ix.sibling(ix.row(), 0).data().toString();

            if (!pk.isEmpty())
                rs2defaults[pk] = ix.data();

            break;
        }
    });

    m3->insertRows(0, 4, m3->index(0, 0));
    m3->setData(m3->index(0, 0), "LAM"); m3->setData(m3->index(0, 1), mat[10]);
    m3->setData(m3->index(1, 0), "BIN"); m3->setData(m3->index(1, 1), mat[0]);
    m3->setData(m3->index(2, 0), "UNB"); m3->setData(m3->index(2, 1), mat[2]);
    m3->setData(m3->index(3, 0), "BOU"); m3->setData(m3->index(3, 1), mat[6]);

    // fourth table
    ui->tableView_4->setItemDelegateForColumn(0, new TableIntDelegate(this));
    ui->tableView_4->setItemDelegateForColumn(1, new TagsDelegate(this));

    auto *m4 = new TableModel({"Chain number", "Default Tags"}, this);

    ui->tableView_4->setModel(m4);

    connect(m4, &TableModel::foo, [this](QModelIndex ix, QVariant old) {
        bool ok;
        int pk;
        switch (ix.column())
        {
        case 0: // chain number
            pk = old.toInt(&ok);

            if (ok)
                cn2defaults.remove(pk);

            cn2defaults.insert(ix.data().toInt(), ix.sibling(ix.row(), 1).data());

            break;
        case 1: // atom color
            pk = ix.sibling(ix.row(), 0).data().toInt(&ok);

            if (ok)
                cn2defaults[pk] = ix.data();

            break;
        }
    });

    m4->insertRows(0, 12, m4->index(0, 0));

    for (int i = 0; i < 12; i++)
    {
        m4->setData(m4->index(i, 0), i);
        m4->setData(m4->index(i, 1), mat[i + 1]);
    }
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

    Q_ASSERT(false);
}

const QVariant& Preferences::resName2color(const QString& resName) const
{
    static const QVariant tmp;
    auto i = rs2defaults.find(resName);
    return i != rs2defaults.end() ? i.value() : tmp;
}

const QVariant& Preferences::chainID2color(uint chainID) const
{
    static const QVariant tmp;
    auto i = cn2defaults.find(chainID);
    return i != cn2defaults.end() ? i.value() : tmp;
}
