#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>
#include <QStyleOptionProgressBar>
#include <QTimer>
#include <QTime>

class StatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit StatusBar(QWidget *parent = nullptr);

    void start();
    void stop();

protected:
    void paintEvent(QPaintEvent *event);

private:
    QString permanent;

    QStyleOptionProgressBar sopb;

    QTimer tm;

    int elapsed;
    QTime t;

    int frame, total;

signals:

public slots:
    void clearPermanentMessage();
    void showPermanentMessage(const QString &text);

    void setFrame(int frame, int total);
    void setProgress(int progress);
};

#endif // STATUSBAR_H
