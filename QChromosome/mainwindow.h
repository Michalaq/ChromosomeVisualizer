#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "../QtChromosomeViz_v2/VizWidget.hpp"
#include "rendersettings.h"

namespace Ui
{
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
    void addLayer();

    void setFrame(int n);
    void updateFrameCount(int n);

    void setSoftMinimum(int min);
    void setSoftMaximum(int max);

    /* animation */
    void start();
    void previous();
    void reverse(bool checked);
    void play(bool checked);
    void next();
    void end();

    /* selection */
    void selectAll();
    void handleSelection(const AtomSelection &selection);
    void handleModelSelection();

    /* actions */
    void setBaseAction(bool enabled);

    /* povray */
    void capture();
    void captureMovie();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    Ui::MainWindow *ui;

    std::shared_ptr<Simulation> simulation;

    int currentFrame;
    int lastFrame;

    QTimer timer;

    QActionGroup *actionGroup;
    QLinkedList<QAction*> modifiers;

    QHash<Qt::Key, QAction*> bindings;
    QHash<Qt::Key, QLinkedList<QAction*>::Iterator> lookup;

    QHash<QObject*, const char*> mappedSlot;

    RenderSettings *renderSettings;

    int softMinimum;
    int softMaximum;
};

#endif // MAINWINDOW_H
