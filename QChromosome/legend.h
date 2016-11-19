#ifndef LEGEND_H
#define LEGEND_H

#include <QLabel>
#include <QAction>

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
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:
    QColor color;

    QAction undergraph;
    QAction visible;

    bool hover;
};

#endif // LEGEND_H
