#ifndef MEDIAPANEL_H
#define MEDIAPANEL_H

#include <QWidget>
#include <QTimer>
#include <QTime>

namespace Ui {
class MediaPanel;
}

class Session;

class MediaPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MediaPanel(QWidget *parent = nullptr);
    ~MediaPanel();

    void setSession(Session* s);

    void playForwards(bool checked);
    void playBackwards(bool checked);

    void goToStart();
    void goToEnd();

    void goToNextFrame();
    void goToPreviousFrame();

private:
    Ui::MediaPanel *ui;
    Session* session;

    QTimer timer;
    QTime time;

    int direction;
friend class MainWindow;
};

#endif // MEDIAPANEL_H
