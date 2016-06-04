#ifndef LAYERATTRIBUTES_H
#define LAYERATTRIBUTES_H

#include <QWidget>

namespace Ui {
    class LayerAttributes;
}

class LayerAttributes : public QWidget
{
    Q_OBJECT
public:
    explicit LayerAttributes(QWidget *parent = 0);
    ~LayerAttributes();

signals:

public slots:

private:
    Ui::LayerAttributes *ui;
};

#endif // LAYERATTRIBUTES_H
