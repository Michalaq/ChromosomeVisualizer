#ifndef LEGEND_H
#define LEGEND_H

#include <QLabel>

class Legend : public QLabel
{
    Q_OBJECT
public:
    explicit Legend(const QString & text, const QColor& color, QWidget *parent = 0);

    QColor pen() const;
    QColor brush() const;

signals:
    void changed();

public slots:

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);

private:
    QColor _color;
};

#endif // LEGEND_H
