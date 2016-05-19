#ifndef DISPLAYPARAMETERSWIDGET_HPP
#define DISPLAYPARAMETERSWIDGET_HPP

#include <QtWidgets>

class VizWidget;

class DisplayParametersWidget : public QWidget
{
    Q_OBJECT

public:
    DisplayParametersWidget(QWidget * parent = nullptr);

    void setVizWidget(VizWidget * widget);

private:
    void initializeControls();
    void initializeSignals();

    VizWidget * vizWidget_;
    QPushButton * backgroundColorButton_;
    QDoubleSpinBox * fogDensitySlider_;
    QDoubleSpinBox * fogContributionSlider_;
    QPushButton * labelBackgroundColorButton_;
    QPushButton * labelTextColorButton_;
};

#endif // DISPLAYPARAMETERSWIDGET_HPP
