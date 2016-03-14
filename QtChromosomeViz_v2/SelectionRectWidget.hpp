#ifndef SELECTIONRECTWIDGET_HPP
#define SELECTIONRECTWIDGET_HPP

#include <QtWidgets>

class SelectionRectWidget : public QWidget
{
    Q_OBJECT

public:
    SelectionRectWidget(QWidget * parent = nullptr);
    void setRectangle(const QRect & r);
    virtual void paintEvent(QPaintEvent * event) override;

private:
    QRect rect_;
};

#endif // SELECTIONRECTWIDGET_HPP
