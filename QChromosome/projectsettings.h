#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include <QWidget>
#include <QFileInfo>

namespace Ui
{
    class ProjectSettings;
}

class ProjectSettings : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectSettings(QWidget *parent = 0);
    ~ProjectSettings();

    int getFPS() const;
    int getDocumentTime() const;
    int getMinimumTime() const;
    int getMaximumTime() const;
    int getPreviewMinTime() const;
    int getPreviewMaxTime() const;
    QString getFileName() const;

    void setDocumentTime(int documentTime);

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

    QFileInfo filePath;

signals:
    void fileNameChanged(const QString&);

public slots:

friend class MainWindow;
friend class MediaPanel;
};

#endif // PROJECTSETTINGS_H
