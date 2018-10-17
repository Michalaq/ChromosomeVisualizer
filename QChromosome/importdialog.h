#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ImportDialog;
}

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDialog(QWidget *parent = 0);
    ~ImportDialog();

    int first() const;
    int last() const;
    int stride() const;

    bool loadInBackground() const;

private:
    Ui::ImportDialog *ui;

    bool unlimited;
};

#endif // IMPORTDIALOG_H
