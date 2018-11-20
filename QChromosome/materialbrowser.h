#ifndef MATERIALBROWSER_H
#define MATERIALBROWSER_H


#include <QMainWindow>
#include "listview.h"

namespace Ui {
class MaterialBrowser;
}

class Material;
class Session;

class MaterialBrowser : public QMainWindow
{
    Q_OBJECT

public:
    ~MaterialBrowser();

    Material *mat[13];

    static MaterialBrowser* getInstance();

    ListView* makeListView();
    void setSession(Session* s);

protected:
    void paintEvent(QPaintEvent *event);

signals:
    void materialsSelected(const QList<Material*>&);

private:
    explicit MaterialBrowser(QWidget *parent = 0);

    Ui::MaterialBrowser *ui;

    static MaterialBrowser* instance;

    ListView* listView;
};

#endif // MATERIALBROWSER_H
