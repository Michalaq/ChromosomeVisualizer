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

    bool eventFilter(QObject *watched, QEvent *event);

public slots:
    void openSimulation();
    void updateFrameCount(int n);

    void start();
    void previous();
    void reverse(bool checked);
    void play(bool checked);
    void next();
    void end();

private:
    Ui::MainWindow *ui;

    int currentFrame;
    int lastFrame;

    QTimer timer;
};

#endif // MAINWINDOW_H
