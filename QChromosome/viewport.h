#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QWidget>

namespace Ui {
    class Viewport;
}

enum AxisPosition
{
    Off_,
    BottomLeft,
    BottomRight,
    TopLeft,
    TopRight
};

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
    AxisPosition getAxisPosition() const;
    double getAxisScale() const;
    bool getAxisTextVisible() const;

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

signals:
    void viewportChanged();

public slots:

private:
    Ui::Viewport *ui;
};

#endif // VIEWPORT_H
