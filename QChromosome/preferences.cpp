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

#include <QJsonDocument>
#include <QJsonArray>

std::vector<int> toVector(const QVariant& variant)
{
    auto tmp = QJsonDocument::fromJson(variant.toByteArray()).array();

    std::vector<int> ans;

    for (auto i : tmp)
        ans.push_back(i.toInt());

    return ans;
}

#include <QSettings>
#include <QStandardPaths>
#include "materialbrowser.h"
#include "tagsdelegate.h"
#include "tablemodel.h"

static const QString unresolved[] = {
    "(unresolved binder type)",
    "(unresolved bead type)"
};

Preferences::Preferences(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Preferences),
    tn2label({{qHash(unresolved[0]), unresolved[0]},
              {qHash(unresolved[1]), unresolved[1]}})
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    ui->TabWidget->tabBar()->setStyle(new MyProxyStyle);

    connect(ui->lineEdit_2, &QLineEdit::editingFinished, [this]() {
        QSettings().setValue("locallib", ui->lineEdit_2->text());
    });

    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        ui->stackedWidget->setCurrentIndex(index);
    });

    QSettings settings;
    ui->lineEdit_2->setText(settings.value("locallib", QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString());

    QVariantList mat[13];

    for (int i = 0; i < 13; i++)
        mat[i] = {QVariant::fromValue(MaterialBrowser::getInstance()->mat[i])};

    // first table
    ui->tableView->setItemDelegateForColumn(0, new TableIntDelegate(this));
    ui->tableView->setItemDelegateForColumn(1, new TableNameDelegate(this));

    auto *m1 = new TableModel({"Binder Type", "Binder Name"}, this);

    ui->tableView->setModel(m1);

    connect(m1, &TableModel::foo, [this](QModelIndex ix, QVariant old) {
        switch (ix.column())
        {
        case 0: // binder type
        {
            auto pk = ix.data().toInt();
            auto oldl = ix.sibling(ix.row(), 1).data();

            if (oldl.isValid())
            {
                auto oldtn = old.isValid() ? bt2tn.take(old.toInt()) : qHash(oldl.toString());
                bt2tn[pk] = oldtn;
            }
        }
            break;
        case 1: // binder name
        {
            auto pk = ix.sibling(ix.row(), 0).data();
            auto newl = ix.data().toString();
            auto newtn = qHash(newl);

            if (pk.isValid())
                bt2tn[pk.toInt()] = newtn;

            tn2label[newtn] = newl;
        }
            break;
        }
    });

    m1->insertRows(0, 2, m1->index(0, 0));
    m1->setData(m1->index(0, 0), 0); m1->setData(m1->index(0, 1), "LAM");
    m1->setData(m1->index(1, 0), 1); m1->setData(m1->index(1, 1), "BIN");

    // second table
    ui->tableView_2->setItemDelegateForColumn(0, new TableVectDelegate(this));
    ui->tableView_2->setItemDelegateForColumn(1, new TableNameDelegate(this));

    auto *m2 = new TableModel({"Energy Vector", "Bead Name"}, this);

    ui->tableView_2->setModel(m2);

    connect(m2, &TableModel::foo, [this](QModelIndex ix, QVariant old) {
        switch (ix.column())
        {
        case 0: // energy vector
        {
            auto pk = toVector(ix.data());
            auto oldl = ix.sibling(ix.row(), 1).data();

            if (oldl.isValid())
            {
                auto oldtn = old.isValid() ? ev2tn.take(toVector(old)) : qHash(oldl.toString());
                ev2tn[pk] = oldtn;
            }
        }
            break;
        case 1: // bead name
        {
            auto pk = ix.sibling(ix.row(), 0).data();
            auto newl = ix.data().toString();
            auto newtn = qHash(newl);

            if (pk.isValid())
                ev2tn[toVector(pk)] = newtn;

            tn2label[newtn] = newl;
        }
            break;
        }
    });

    m2->insertRows(0, 4, m2->index(0, 0));
    m2->setData(m2->index(0, 0), "[0,0]"); m2->setData(m2->index(0, 1), "UNB");
    m2->setData(m2->index(1, 0), "[0,1]"); m2->setData(m2->index(1, 1), "BOU");
    m2->setData(m2->index(2, 0), "[1,0]"); m2->setData(m2->index(2, 1), "LAM");
    m2->setData(m2->index(3, 0), "[2,0]"); m2->setData(m2->index(3, 1), "LAM");

    // third table
    ui->tableView_3->setItemDelegateForColumn(0, new TableNameDelegate(this));
    ui->tableView_3->setItemDelegateForColumn(1, new TagsDelegate(this));

    auto *m3 = new TableModel({"Residue Name", "Default Tags"}, this);

    ui->tableView_3->setModel(m3);

    connect(m3, &TableModel::foo, [this](QModelIndex ix, QVariant old) {
        switch (ix.column())
        {
        case 0: // residue name
        {
            auto newl = ix.data().toString();
            auto newtn = qHash(newl);
            auto oldc = ix.sibling(ix.row(), 1).data();

            if (oldc.isValid())
                tn2defaults[newtn] = old.isValid() ? tn2defaults.take(qHash(old.toString())) : oldc;

            tn2label[newtn] = newl;
        }
            break;
        case 1: // atom color
        {
            auto oldl = ix.sibling(ix.row(), 0).data();
            auto newc = ix.data();

            if (oldl.isValid())
                tn2defaults[qHash(oldl.toString())] = newc;
        }
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

uint Preferences::bt2typename(int bt)
{
    auto i = bt2tn.find(bt);
    return i != bt2tn.end() ? i.value() : qHash(unresolved[0]);
}

uint Preferences::ev2typename(std::vector<int> ev)
{
    auto i = ev2tn.find(ev);
    return i != ev2tn.end() ? i.value() : qHash(unresolved[1]);
}

uint Preferences::rs2typename(const QString &rs)
{
    auto ans = qHash(rs);
    tn2label[ans] = rs;
    return ans;
}

const QString& Preferences::typename2label(uint tn)
{
    return tn2label[tn];
}

const QVariant& Preferences::typename2color(uint tn)
{
    return tn2defaults[tn];
}

const QVariant& Preferences::chainnumber2color(int cn)
{
    static const QVariant tmp;
    auto i = cn2defaults.find(cn);
    return i != cn2defaults.end() ? i.value() : tmp;
}
