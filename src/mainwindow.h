#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "opengl/vizwidget.h"
#include "rendersettings.h"
#include "preferences.h"
#include "materialbrowser.h"
#include "moviemaker.h"
#include "camera.h"
#include "pickwidget.h"
#include "attributes.h"
#include "session.h"
#include "opengl/defaultlight.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QStringList& paths, QWidget *parent = 0);
    ~MainWindow();

    bool eventFilter(QObject *watched, QEvent *event);

public slots:
    void newProject();
    void openProject(const QString& path = "");

    void saveProject();
    void saveProjectAs();

    void addLayer();

    /* selection */
    void selectAll();
    void handleSceneSelection(const QItemSelection &selected, QItemSelectionModel::SelectionFlags flags);
    void handleModelSelection();

    /* actions */
    void setBaseAction(bool enabled);
    void updateLocks();

    /* animation */
    void recordActiveObjects();
    void autokeying(bool checked);

    void playForwards(bool checked);
    void playBackwards(bool checked);

    void goToStart();
    void goToEnd();

    void goToNextFrame();
    void goToPreviousFrame();

    /* povray */
    void render() const;

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

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
    MovieMaker* movieMaker;

    void addCamera(Camera *camera);
    void changeCamera(Camera* camera);

    PickWidget* pw;

    MetaAttributes* recent;

    Session* makeSession();
    void setCurrentSession(Session* s);

    Session* session;
    QActionGroup* sessions;
    QList<Session*> remaining;

    DefaultLight* light;

    QAction* separator;
};

#endif // MAINWINDOW_H
