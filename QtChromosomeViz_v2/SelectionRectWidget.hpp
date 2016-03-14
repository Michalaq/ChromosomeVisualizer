#ifndef SELECTIONRECTWIDGET_HPP
#define SELECTIONRECTWIDGET_HPP

#include <QtWidgets>

class SelectionRectWidget : public QWidget
{
    Q_OBJECT

public:
    SelectionRectWidget(QWidget * parent = nullptr);
    void setRectangle(const QRectF & r);
    virtual void paintEvent(QPaintEvent * event) override;

private:
    QRectF rect_;
};

#endif // SELECTIONRECTWIDGET_HPP
