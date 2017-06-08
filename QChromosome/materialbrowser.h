#ifndef MATERIALBROWSER_H
#define MATERIALBROWSER_H

#include <QWidget>

namespace Ui {
class MaterialBrowser;
}

class MaterialBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit MaterialBrowser(QWidget *parent = 0);
    ~MaterialBrowser();

private:
    Ui::MaterialBrowser *ui;
};

#endif // MATERIALBROWSER_H
