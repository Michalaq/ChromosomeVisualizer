#ifndef LEGEND_H
#define LEGEND_H

#include <QLabel>

class Legend : public QLabel
{
    Q_OBJECT
public:
    explicit Legend(const QString & text, QWidget *parent = 0);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // LEGEND_H
