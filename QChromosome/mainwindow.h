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

    /* animation */
    void start();
    void previous();
    void reverse(bool checked);
    void play(bool checked);
    void next();
    void end();

    /* selection */
    void selectAll();
    void handleSelection(const AtomCollection& selected);

private:
    Ui::MainWindow *ui;

    std::shared_ptr<Simulation> simulation;

    int currentFrame;
    int lastFrame;

    QTimer timer;

    /* cf. QTBUG-2982 */
    void cacheProperties(QWidget* widget, QHash<QString, QHash<QString, QHash<QString, QVariant> > > cache = {});
};

#endif // MAINWINDOW_H
