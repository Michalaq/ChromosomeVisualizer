#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openSimulation();
    void updateFrameCount(int n);

    void bb();
    void ab();
    void pb(bool f);
    void pf(bool f);
    void af();
    void ff();

private:
    Ui::MainWindow *ui;

    int lastFrame;

    QTimer timer;
};

#endif // MAINWINDOW_H
