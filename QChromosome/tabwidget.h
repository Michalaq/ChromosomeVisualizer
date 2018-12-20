#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QTextStream>
#include <QDir>

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

    bool openFile() const;
    bool render() const;

    bool saveOutput() const;
    bool saveTraslator() const;

    QDir getOutputDir() const;
    QDir getTranslatorDir() const;

    QString getOutputName() const;
    QString getTranslatorName() const;

    QPair<int, int> frameRange() const;

    QString getExtension() const;

    void getFFmpegArgs(QStringList& argv) const;

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

protected:
    void connectNotify(const QMetaMethod& signal);

private:
    Ui::TabWidget *ui;
    Session* session;

    qreal resolution;
    QSize imageResolution;
    qreal aspectRatio;

    qreal widthUnit;
    qreal resolutionUnit;

    int startFrame;
    int endFrame;

    void setFrameRange(int min, int max);

    JPEGDialog* jpegSettings;
    TARGADialog* targaSettings;

    const QString defaultPath;

    QString ext;

signals:
    void filmRatioChanged(double);

public slots:
    void updateFrames();
};

#endif // TABWIDGET_H
