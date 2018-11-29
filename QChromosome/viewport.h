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
    QRgb ucBackgroundColor      __attribute__((aligned(4)));
    QRgb ucEnvironmentColor     __attribute__((aligned(4)));
    float ufEnvironmentStrength __attribute__((aligned(4)));
    bool ubEnableFog            __attribute__((aligned(1)));
    QRgb ucFogColor             __attribute__((aligned(4)));
    float ufFogStrength         __attribute__((aligned(4)));
    float ufFogDistance         __attribute__((aligned(4)));
}                               __attribute__((aligned(16)));

class Session;

class Viewport : public QWidget
{
    Q_OBJECT
public:
    explicit Viewport(Session* s, QWidget *parent = 0);
    ~Viewport();

    double getSFOpacity() const;
    QColor getSFColor() const;
    bool getSFVisible() const;
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
    Session* session;
};

#endif // VIEWPORT_H
