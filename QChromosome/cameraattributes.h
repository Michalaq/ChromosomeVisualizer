#ifndef CAMERAATTRIBUTES_H
#define CAMERAATTRIBUTES_H

#include <QWidget>
#include <QModelIndexList>

namespace Ui {
class CameraAttributes;
}

class TreeModel;

class CameraAttributes : public QWidget
{
    Q_OBJECT

public:
    explicit CameraAttributes(QWidget *parent = 0);
    ~CameraAttributes();

    void setSelection(TreeModel* selectedModel, const QModelIndexList& selectedRows);
    void unsetSelection();

public slots:


private:
    Ui::CameraAttributes *ui;

    TreeModel *model;
    QModelIndexList rows;

    void updateModelSelection();
    void updatePosition();
};

#endif // CAMERAATTRIBUTES_H
