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

private:
    Ui::MaterialAttributes *ui;
    QList<Material*> selection;
};

#endif // MATERIALATTRIBUTES_H
