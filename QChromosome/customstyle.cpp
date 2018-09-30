#include "customstyle.h"

CustomStyle::CustomStyle(QStyle *style) : QProxyStyle(style)
{

}

void CustomStyle::drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    //TODO here should be implemented MenuItem and PushButton but since QSS+QStyle is not supported, it must be postponed until future release
    switch (control)
    {
    default:
        QProxyStyle::drawComplexControl(control, option, painter, widget);
    }
}
