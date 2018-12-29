#include "messagehandler.h"

MessageHandler* MessageHandler::instance = nullptr;

MessageHandler::MessageHandler(QObject *parent) : QAbstractTableModel(parent)
{

}

MessageHandler* MessageHandler::getInstance()
{
    return instance ? instance : instance = new MessageHandler;
}

int MessageHandler::rowCount(const QModelIndex &) const
{
    return messages.count();
}

int MessageHandler::columnCount(const QModelIndex &) const
{
    return 6;
}

#include <QIcon>

QVariant MessageHandler::data(const QModelIndex &index, int role) const
{
    static QIcon info;
    info.addPixmap(QPixmap(":/error/info"), QIcon::Normal);
    info.addPixmap(QPixmap(":/error/info"), QIcon::Selected);

    static QIcon warning;
    warning.addPixmap(QPixmap(":/error/warning"), QIcon::Normal);
    warning.addPixmap(QPixmap(":/error/warning"), QIcon::Selected);

    static QIcon critical;
    critical.addPixmap(QPixmap(":/error/critical"), QIcon::Normal);
    critical.addPixmap(QPixmap(":/error/critical"), QIcon::Selected);

    switch (index.column())
    {
    case 0:
        if (role == Qt::DecorationRole)
            switch (messages[index.row()].type)
            {
            case QtInfoMsg:
                return info;
            case QtWarningMsg:
                return warning;
            case QtCriticalMsg:
                return critical;
            default:
                return QVariant();
            }
        else
            return QVariant();
    case 1:
        if (role == Qt::DisplayRole)
            return messages[index.row()].description;
        else
            return QVariant();
    case 2:
        if (role == Qt::DisplayRole)
            return messages[index.row()].file;
        else
            return QVariant();
    case 3:
        if (role == Qt::DisplayRole)
            return messages[index.row()].project;
        else
            return QVariant();
    case 4:
        if (role == Qt::DisplayRole && messages[index.row()].line >= 0)
            return messages[index.row()].line;
        else
            return QVariant();
    case 5:
        if (role == Qt::DisplayRole && messages[index.row()].column >= 0)
            return messages[index.row()].column;
        else
            return QVariant();
    default:
        return QVariant();
    }
}

QVariant MessageHandler::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch (section)
    {
    case 0:
        return "";
    case 1:
        return "Desription";
    case 2:
        return "File";
    case 3:
        return "Project";
    case 4:
        return "Line";
    case 5:
        return "Column";
    default:
        return  QVariant();
    }
}

void MessageHandler::setProject(const QString &p)
{
    project = p;
}

void MessageHandler::handleMessage(QtMsgType type, const QString &description, const QString &file, int line, int column)
{
    beginInsertRows(QModelIndex(), messages.count(), messages.count());
    messages.append({type, description, file, project, line, column});
    endInsertRows();
}

void qcInfo(const QString& description, const QString& file, int line, int column)
{
    MessageHandler::getInstance()->handleMessage(QtInfoMsg, description, file, line, column);
}

void qcWarning(const QString& description, const QString& file, int line, int column)
{
    MessageHandler::getInstance()->handleMessage(QtWarningMsg, description, file, line, column);
}

void qcCritical(const QString& description, const QString& file, int line, int column)
{
    MessageHandler::getInstance()->handleMessage(QtCriticalMsg, description, file, line, column);
}
