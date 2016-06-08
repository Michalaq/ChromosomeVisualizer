#ifndef CAMERASETTINGS_H
#define CAMERASETTINGS_H

#include <QWidget>

class Camera;

namespace Ui
{
    class CameraSettings;
}

class CameraSettings : public QWidget
{
    Q_OBJECT
public:
    explicit CameraSettings(Camera *parent);
    ~CameraSettings();

signals:

public slots:

private:
    Ui::CameraSettings *ui;

    Camera *camera;

friend class Camera;
};

#endif // CAMERASETTINGS_H
