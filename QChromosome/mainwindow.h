#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "../QtChromosomeViz_v2/VizWidget.hpp"

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

    void setFrame(int n);
    void updateFrameCount(int n);

    void start();
    void previous();
    void reverse(bool checked);
    void play(bool checked);
    void next();
    void end();

    void handleSelection(const AtomSelection & selection);

private:
    Ui::MainWindow *ui;

    int currentFrame;
    int lastFrame;

    QTimer timer;

    /* cf. QTBUG-2982 */
    void cacheProperties(QWidget* widget, QHash<QString, QHash<QString, QHash<QString, QVariant> > > cache);
};

#endif // MAINWINDOW_H
