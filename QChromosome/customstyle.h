#ifndef CUSTOMSTYLE_H
#define CUSTOMSTYLE_H

#include <QProxyStyle>

class CustomStyle : public QProxyStyle
{
    Q_OBJECT
public:
    explicit CustomStyle(QStyle *style = nullptr);

    virtual void drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;

signals:

public slots:
};

#endif // CUSTOMSTYLE_H
