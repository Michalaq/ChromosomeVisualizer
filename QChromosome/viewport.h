#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QWidget>

#include "../QtChromosomeViz_v2/VizWidget.hpp"
#include "axis.h"

namespace Ui {
    class Viewport;
}

class Viewport : public QWidget
{
    Q_OBJECT
public:
    explicit Viewport(QWidget *parent = 0);
    ~Viewport();

    QColor getBackgroundColor() const;
    QColor getLabelTextColor() const;
    QColor getLabelBackgroundColor() const;
    double getSFOpacity() const;
    QColor getSFColor() const;
    bool getSFVisible() const;
    double getFogDensity() const;
    double getFogContribution() const;

    void setAxis(Axis *axis);

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

signals:
    void viewportChanged();

public slots:

private:
    Ui::Viewport *ui;

    VizWidget* vizWidget_;
    Axis *axis_;
};

#endif // VIEWPORT_H
