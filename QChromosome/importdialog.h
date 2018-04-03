#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ImportDialog;
}

class SimulationLayer;

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDialog(SimulationLayer* sl, QWidget *parent = 0);
    ~ImportDialog();

private:
    Ui::ImportDialog *ui;

    SimulationLayer* simulationLayer;
};

#endif // IMPORTDIALOG_H
