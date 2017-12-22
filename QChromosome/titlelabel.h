#ifndef TITLELABEL_H
#define TITLELABEL_H

#include <QLabel>

class TitleLabel : public QLabel
{
    Q_OBJECT
public:
    explicit TitleLabel(QWidget *parent = nullptr);

    void setTitle(const QString& t);
    void setElements(const QString& e);

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:

private:
    QString title, elements;
};

#endif // TITLELABEL_H
