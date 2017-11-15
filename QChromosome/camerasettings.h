#ifndef CAMERASETTINGS_H
#define CAMERASETTINGS_H

#include <QWidget>
#include <QModelIndex>

class Camera;

namespace Ui
{
    class CameraSettings;
}

class CameraSettings : public QWidget
{
    Q_OBJECT
public:
    explicit CameraSettings(QWidget *parent = 0);
    ~CameraSettings();

    void handleSelection(const QModelIndexList& selection);
    void setRotationType(int rt);

protected:
    void resizeEvent(QResizeEvent *event);

signals:

public slots:
    void updateModelView();
    void updateProjection();

private:
    Ui::CameraSettings *ui;

    QList<Camera*> cameras;

    QString title, list;

friend class MainWindow;
};

#endif // CAMERASETTINGS_H
