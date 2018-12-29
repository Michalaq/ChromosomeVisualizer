#ifndef MYPROXYSTYLE_H
#define MYPROXYSTYLE_H

#include <QProxyStyle>

class MyProxyStyle : public QProxyStyle
{
    Q_OBJECT
public:
    void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled, const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const;
};

#endif // MYPROXYSTYLE_H
