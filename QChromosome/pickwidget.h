#ifndef PICKWIDGET_H
#define PICKWIDGET_H

#include "lineedit.h"

class PickWidget : public LineEdit
{
    Q_OBJECT
public:
    explicit PickWidget(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // PICKWIDGET_H
