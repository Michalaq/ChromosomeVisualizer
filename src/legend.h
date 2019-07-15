#ifndef LEGEND_H
#define LEGEND_H

#include <QLabel>
#include <QtCharts/QAbstractSeries>

class Legend : public QLabel
{
    Q_OBJECT
public:
    explicit Legend(QtCharts::QAbstractSeries *s, QWidget *parent = 0);
    ~Legend();

    QColor pen() const;
    QColor brush() const;

signals:
    void changed();

public slots:

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:
    QtCharts::QAbstractSeries *series;
    bool hover;
};

#endif // LEGEND_H
