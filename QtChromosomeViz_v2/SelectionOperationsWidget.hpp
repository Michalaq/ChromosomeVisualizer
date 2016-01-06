#ifndef SELECTIONOPERATIONSWIDGET_HPP
#define SELECTIONOPERATIONSWIDGET_HPP

#include <QtGui>
#include <QtWidgets>

class VizWidget;

class SelectionOperationsWidget : public QWidget
{
    Q_OBJECT;

public:
    SelectionOperationsWidget(VizWidget * vizWidget, QWidget * parent = nullptr);
    virtual ~SelectionOperationsWidget();

private:
    void initializeControls();
    void initializeSignals();

    VizWidget * vizWidget_;

    QPushButton * selectAllButton_;
    QPushButton * selectTypeButton_;

    QPushButton * setColorButton_;
    QPushButton * setOpacityButton_;
    QPushButton * setSizeButton_;
};

#endif // SELECTIONOPERATIONSWIDGET_HPP
