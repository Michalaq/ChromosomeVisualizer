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

    int getDocumentTime() const;
    QString getFileName() const;

    bool getSaveFileName();
    bool getNewSaveFileName();

    void writeSaveFile(const QJsonDocument &project);

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

    static const char * suffix;
    static const char * version;

private:
    Ui::ProjectSettings *ui;

    QFileInfo filePath;

signals:

public slots:

friend class MainWindow;
};

#endif // PROJECTSETTINGS_H
