#include "materialbrowser.h"
#include "ui_materialbrowser.h"

MaterialBrowser::MaterialBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaterialBrowser),
    s(new QSplitter),
    lv(new QListView),
    w(new QWidget)
{
    ui->setupUi(this);

    lv->setStyleSheet("QLineEdit {\
                      color: white;\
                      border: none;\
                      background: #007acc;\
                  }");

    s->addWidget(w);
    s->addWidget(lv);
    layout()->addWidget(s);

    MaterialListModel * m = new MaterialListModel({});
    m->insertRows(0,3);
    m->setData(m->index(0), "addf");
    m->setData(m->index(1), "addf");
    m->setData(m->index(2), "addf");

    lv->setModel(m);
}

MaterialBrowser::~MaterialBrowser()
{
    delete ui;
}

int MaterialListModel::rowCount(const QModelIndex &parent) const
{
    return stringList.count();
}

QVariant MaterialListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= stringList.size())
        return QVariant();

    if (role == Qt::DisplayRole)
        return stringList.at(index.row());
    else
        return QVariant();
}

QVariant MaterialListModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

Qt::ItemFlags MaterialListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool MaterialListModel::setData(const QModelIndex &index,
                              const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {

        stringList.replace(index.row(), value.toString());
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

bool MaterialListModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        stringList.insert(position, "");
    }

    endInsertRows();
    return true;
}

bool MaterialListModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        stringList.removeAt(position);
    }

    endRemoveRows();
    return true;
}
