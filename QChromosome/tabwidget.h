#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QTextStream>

namespace Ui {
class TabWidget;
}

class Session;
class JPEGDialog;
class TARGADialog;

class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit TabWidget(Session* s, QWidget *parent = nullptr);
    ~TabWidget();

    void setDocumentTime(int time);
    void setMinimumTime(int time);
    void setMaximumTime(int time);
    void setPreviewMinTime(int time);
    void setPreviewMaxTime(int time);
    void setLastFrame(int time);

    QTextStream& operator<<(QTextStream &stream) const;

protected:
    void connectNotify(const QMetaMethod& signal);

private:
    Ui::TabWidget *ui;
    Session* session;

    qreal resolution;
    QSize imageResolution;

    qreal widthUnit;
    qreal resolutionUnit;

    int startFrame;
    int endFrame;

    void setFrameRange(int min, int max);

    JPEGDialog* jpegSettings;
    TARGADialog* targaSettings;

    const QString defaultPath;

signals:
    void filmRatioChanged(double);

public slots:
    void updateFrames();
};

#endif // TABWIDGET_H
