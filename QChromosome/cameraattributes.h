#ifndef CAMERAATTRIBUTES_H
#define CAMERAATTRIBUTES_H

#include <QWidget>

namespace Ui {
class CameraAttributes;
}

class CameraAttributes : public QWidget
{
    Q_OBJECT

public:
    explicit CameraAttributes(QWidget *parent = 0);
    ~CameraAttributes();

private:
    Ui::CameraAttributes *ui;
};

#endif // CAMERAATTRIBUTES_H
