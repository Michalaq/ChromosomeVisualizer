#ifndef MEDIAPANEL_H
#define MEDIAPANEL_H

#include <QWidget>
#include <QMutex>
#include <QTimer>

namespace Ui {
class MediaPanel;
}

class Session;
class Camera;
class MainWindow;

class MediaPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MediaPanel(Session* s, MainWindow* w, QWidget *parent = nullptr);
    ~MediaPanel();

    void recordActiveObjects();
    void autokeying(bool checked);

    void playForwards(bool checked);
    void playBackwards(bool checked);

    void goToStart();
    void goToEnd();

    void goToNextFrame();
    void goToPreviousFrame();

    void setFPS(int fps);
    void setDocumentTime(int time);
    void setMinimumTime(int time);
    void setMaximumTime(int time);
    void setPreviewMinTime(int time);
    void setPreviewMaxTime(int time);
    void setLastFrame(int time);

    void changeCamera(Camera* camera);

    void pause();
    void resume();

protected:
    void timerEvent(QTimerEvent* event);

private:
    Ui::MediaPanel *ui;
    Session* session;

    QMutex mutex;
    QTimer timer;
};

#endif // MEDIAPANEL_H
