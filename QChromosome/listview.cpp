#include "listview.h"
#include "material.h"

ListView::ListView(QWidget *parent) : QListView(parent)
{

}

ListView::~ListView()
{

}

#include <QMouseEvent>

void ListView::mousePressEvent(QMouseEvent *event)
{
    if (indexAt(event->pos()).isValid())
        clicked = true;
    else
        setCurrentIndex(QModelIndex());

    QListView::mousePressEvent(event);
}

#include <QMimeData>
#include <QDrag>

void ListView::mouseMoveEvent(QMouseEvent *event)
{
    if (clicked)
    {
        clicked = false;

        auto index = indexAt(event->pos());

        if (index.isValid())
        {
            QDrag *drag = new QDrag(index.data(Qt::DecorationRole).value<Material*>());
            drag->setMimeData(new QMimeData);
            drag->exec(Qt::CopyAction | Qt::MoveAction);
        }
    }
    else
        QListView::mouseMoveEvent(event);
}

void ListView::keyPressEvent(QKeyEvent *event)
{
    QListView::keyPressEvent(event);

    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
        model()->removeRow(currentIndex().row());
}

MaterialListModel::MaterialListModel(QObject *parent) : QAbstractListModel(parent)
{

}

MaterialListModel::~MaterialListModel()
{
    for (auto i : materials)
        delete i;
}

int MaterialListModel::rowCount(const QModelIndex &) const
{
    return materials.count();
}

QVariant MaterialListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= materials.size())
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
        return materials.at(index.row())->getName();
    case Qt::DecorationRole:
        return QVariant::fromValue(materials.at(index.row()));
    default:
        return QVariant();
    }
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
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::EditRole:
            materials.at(index.row())->setName(value.toString());
            emit dataChanged(index, index);
            return true;
        case Qt::DecorationRole:
            materials.replace(index.row(), value.value<Material*>());
            emit dataChanged(index, index);
            return true;
        }
    }

    return false;
}

bool MaterialListModel::insertRows(int position, int rows, const QModelIndex &)
{
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row)
        materials.insert(position, 0);

    endInsertRows();
    return true;
}

bool MaterialListModel::removeRows(int position, int rows, const QModelIndex &)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row)
    {
        auto m = materials.takeAt(position);
        id2mat.remove(m->getId());
        m->deleteLater();
    }

    endRemoveRows();
    return true;
}

void MaterialListModel::prepend(Material *m)
{
    beginInsertRows(QModelIndex(), 0, 0);

    materials.prepend(m);
    id2mat.insert(m->getId(), m);

    endInsertRows();
}

#include <QJsonObject>
#include <QJsonArray>

void MaterialListModel::read(const QJsonArray &json)
{
    for (auto i = json.begin(); i != json.end(); i++)
    {
        auto * m = new Material;
        m->read(i->toObject());
        prepend(m);
    }
}

void MaterialListModel::write(QJsonArray &json) const
{
    for (auto i = materials.begin(); i != materials.end(); i++)
    {
        QJsonObject m;
        (*i)->write(m);
        json.prepend(m);
    }
}

#include <QRegularExpression>
#include <QSet>

QString MaterialListModel::next_name()
{
    static const QRegularExpression re("^Mat(.(?<label>[1-9][0-9]*))?$");

    QSet<int> used;

    for (auto * m : materials)
    {
        QRegularExpressionMatch match = re.match(m->getName());

        if (match.hasMatch())
            used.insert(match.captured("label").toInt());
    }

    auto values = used.values();
    qSort(values);

    int i = 0;

    for (auto j = values.begin(); j != values.end() && i == *j; i++, j++);

    return i ? QString("Mat.") + QString::number(i) : "Mat";
}

Material* MaterialListModel::getMaterialById(const QUuid &id) const
{
    return id2mat[id];
}

MaterialDelegate::MaterialDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

#include <QLineEdit>

void MaterialDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    qobject_cast<QLineEdit*>(editor)->setText(index.data().toString());
    editor->setFocus();
}

void MaterialDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QString name = qobject_cast<QLineEdit*>(editor)->text();

    if (!name.isEmpty())
        model->setData(index, name);
}

#include <QPainter>

void MaterialDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();
    index.data(Qt::DecorationRole).value<Material*>()->paint(painter, QRect(option.rect.topLeft() + QPoint(10, 1), QSize(30, 30)));
    painter->restore();
}
