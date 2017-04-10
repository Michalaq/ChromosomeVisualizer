#ifndef CAMERASETTINGS_H
#define CAMERASETTINGS_H

#include <QWidget>
#include "camera.h"

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

    void setCamera(Camera *c);
    void setRotationType(int rt);

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

signals:

public slots:
    void updateModelView();

private:
    Ui::CameraSettings *ui;

    Camera *camera;

friend class MainWindow;
};

#endif // CAMERASETTINGS_H
