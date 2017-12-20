#ifndef TITLELABEL_H
#define TITLELABEL_H

#include <QLabel>

class TitleLabel : public QLabel
{
    Q_OBJECT
public:
    explicit TitleLabel(QWidget *parent = nullptr);

    void setContents(const QString &t, const QString &l);

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:

private:
    QString title, list;
};

#endif // TITLELABEL_H
