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

struct viewport_data_t
{
    float ufFogDensity      __attribute__((aligned(4)));
    float ufFogContribution __attribute__((aligned(4)));
    QRgb ucBackgroundColor  __attribute__((aligned(4)));
}                           __attribute__((aligned(16)));

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

    static const viewport_data_t& getBuffer();
    static bool modified;

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

signals:
    void viewportChanged();

public slots:

private:
    Ui::Viewport *ui;

    static viewport_data_t buffer;
};

#endif // VIEWPORT_H
