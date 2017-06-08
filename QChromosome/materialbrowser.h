#ifndef MATERIALBROWSER_H
#define MATERIALBROWSER_H

#include <QWidget>
#include <QSplitter>
#include <QListView>

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

    QSplitter *s;
    QListView *lv;
    QWidget * w;
};

#endif // MATERIALBROWSER_H
