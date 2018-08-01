#ifndef MEDIAPANEL_H
#define MEDIAPANEL_H

#include <QWidget>

namespace Ui {
class MediaPanel;
}

class MediaPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MediaPanel(QWidget *parent = nullptr);
    ~MediaPanel();

private:
    Ui::MediaPanel *ui;
friend class MainWindow;
};

#endif // MEDIAPANEL_H
