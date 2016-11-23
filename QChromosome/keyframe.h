#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <QWidget>

namespace Ui {
    class Keyframes;
}

class Keyframes : public QWidget
{
    Q_OBJECT
public:
    explicit Keyframes(QWidget *parent = 0);
    ~Keyframes();

signals:

public slots:

private:
    Ui::Keyframes *ui;
};

#endif // KEYFRAME_H
