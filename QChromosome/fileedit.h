#ifndef FILEEDIT_H
#define FILEEDIT_H

#include "lineedit.h"
#include "mediacontrol.h"

class FileEdit : public LineEdit
{
    Q_OBJECT
public:
    explicit FileEdit(QWidget *parent = 0);
    ~FileEdit();

    enum Type
    {
        FE_File,
        FE_Directory
    };

    void setType(Type t);

signals:

public slots:

private:
    MediaControl* widget;
    Type type;
};

#endif // FILEEDIT_H
