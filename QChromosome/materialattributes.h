#ifndef MATERIALATTRIBUTES_H
#define MATERIALATTRIBUTES_H

#include <QWidget>

namespace Ui {
class MaterialAttributes;
}

class Material;

class MaterialAttributes : public QWidget
{
    Q_OBJECT

public:
    explicit MaterialAttributes(QWidget *parent = 0);
    ~MaterialAttributes();

    void handleSelection(const QList<Material*> &selected);

protected:
    void paintEvent(QPaintEvent *event);

signals:
    void attributesChanged(const Material*);

private:
    Ui::MaterialAttributes *ui;
    Material* selection;

    QString title;
};

#endif // MATERIALATTRIBUTES_H
