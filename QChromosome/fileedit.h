#ifndef FILEEDIT_H
#define FILEEDIT_H

#include "lineedit.h"
#include "mediacontrol.h"

class FileEdit : public LineEdit
{
    Q_OBJECT
    Q_PROPERTY(Type type WRITE setType)
    Q_ENUMS(Type)
public:
    explicit FileEdit(QWidget *parent = 0);
    ~FileEdit();

    enum Type
    {
        FE_ExistingDirectory,
        FE_OpenFileName,
        FE_SaveFileName
    };

    void setType(Type t);

    void setReadOnly(bool);

signals:

public slots:

private:
    MediaControl* widget;
    Type type;
};

#endif // FILEEDIT_H
