#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "../QtChromosomeViz_v2/VizWidget.hpp"
#include "rendersettings.h"
#include "preferences.h"
#include "materialbrowser.h"
#include "camera.h"
#include "pickwidget.h"
#include "attributes.h"

class Session;

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

    void updateFrameCount(int n);

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
    void handleSceneSelection(const QItemSelection &selected, QItemSelectionModel::SelectionFlags flags);
    void handleModelSelection(const QItemSelection &selected, const QItemSelection &deselected);

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

    Session *session;

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

    void addCamera(Camera *camera);

    PickWidget* pw;
    QLabel* msg;

    MetaAttributes* recent;

    friend class Session;
};

#endif // MAINWINDOW_H
