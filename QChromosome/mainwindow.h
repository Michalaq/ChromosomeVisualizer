#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "../QtChromosomeViz_v2/VizWidget.hpp"
#include "rendersettings.h"
#include "preferences.h"
#include "materialbrowser.h"
#include "camera.h"

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

    void read(const QJsonObject& json);

public slots:
    void newProject();
    void openProject();
    void addLayer();
    void saveProject();
    void saveProjectAs();

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

    void reverse_previous();
    void play_next();

    /* selection */
    void selectAll();
    void handleSelection(const AtomSelection &selection, bool b = true);
    void handleModelSelection();
    void focusSelection(const AtomSelection &s);

    /* actions */
    void setBaseAction(bool enabled);

    /* povray */
    void capture() const;
    void captureMovie() const;

    void updateLocks();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    std::shared_ptr<Simulation> simulation;

    int currentFrame;
    int lastFrame;

    QTimer timer;
    QTime time;

    QActionGroup *actionGroup;
    QLinkedList<QAction*> modifiers;

    QHash<Qt::Key, QAction*> bindings;
    QHash<Qt::Key, QLinkedList<QAction*>::Iterator> lookup;

    QHash<QObject*, Camera::Action> mappedSlot;

    RenderSettings *renderSettings;
    Preferences *preferences;
    MaterialBrowser *materialBrowser;

    int softMinimum;
    int softMaximum;

    QString currentFile;

    void addCamera(Camera *camera);
};

#endif // MAINWINDOW_H
