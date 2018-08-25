#ifndef TARGADIALOG_H
#define TARGADIALOG_H

#include <QDialog>

namespace Ui {
class TARGADialog;
}

class TARGADialog : public QDialog
{
    Q_OBJECT

public:
    explicit TARGADialog(QWidget *parent = nullptr);
    ~TARGADialog();

private:
    Ui::TARGADialog *ui;

    int index;
};

#endif // TARGADIALOG_H
