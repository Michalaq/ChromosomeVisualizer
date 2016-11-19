#ifndef FILEEDIT_H
#define FILEEDIT_H

#include "lineedit.h"

class FileEdit : public LineEdit
{
    Q_OBJECT
public:
    explicit FileEdit(QWidget *parent = 0);
    ~FileEdit();

signals:

public slots:
};

#endif // FILEEDIT_H
