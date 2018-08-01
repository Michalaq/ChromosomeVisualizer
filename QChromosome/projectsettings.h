#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include <QWidget>
#include <QFileInfo>

namespace Ui
{
    class ProjectSettings;
}

class Session;

class ProjectSettings : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectSettings(Session* s, QWidget *parent = 0);
    ~ProjectSettings();

    int getFPS() const;
    int getDocumentTime() const;
    int getMinimumTime() const;
    int getMaximumTime() const;
    int getPreviewMinTime() const;
    int getPreviewMaxTime() const;
    QString getFileName() const;

    void setFPS(int fps);
    void setDocumentTime(int time);
    void setMinimumTime(int time);
    void setMaximumTime(int time);
    void setPreviewMinTime(int time);
    void setPreviewMaxTime(int time);
    void setLastFrame(int time);

    bool getOpenFileName();
    bool getSaveFileName();
    bool getNewSaveFileName();

    QJsonDocument readSaveFile() const;
    void writeSaveFile(const QJsonDocument &project);

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

    static const char * suffix;
    static const char * version;

private:
    Ui::ProjectSettings *ui;
    Session* session;

    QFileInfo filePath;

signals:
    void fileNameChanged(const QString&);

public slots:

};

#endif // PROJECTSETTINGS_H
