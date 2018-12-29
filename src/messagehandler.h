#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H


#include <QAbstractListModel>

struct MessageLog
{
    QtMsgType type;
    QString description;
    QString file;
    QString project;
    int line;
    int column;
};

class MessageHandler : public QAbstractTableModel
{
    Q_OBJECT
public:
    static MessageHandler* getInstance();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void setProject(const QString& p);
    void handleMessage(QtMsgType type, const QString& description, const QString& file, int line, int column);

signals:

public slots:

private:
    explicit MessageHandler(QObject *parent = nullptr);

    static MessageHandler* instance;

    QString project;
    QVector<MessageLog> messages;
};

void qcInfo(const QString& description, const QString& file, int line, int column);
void qcWarning(const QString& description, const QString& file, int line, int column);
void qcCritical(const QString& description, const QString& file, int line, int column);

#endif // MESSAGEHANDLER_H
