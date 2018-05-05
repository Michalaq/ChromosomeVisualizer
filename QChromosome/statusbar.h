#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>

class StatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit StatusBar(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QString permanent;

signals:

public slots:
    void clearPermanentMessage();
    void showPermanentMessage(const QString &text);
};

#endif // STATUSBAR_H
