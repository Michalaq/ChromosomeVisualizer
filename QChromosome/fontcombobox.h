#ifndef FONTCOMBOBOX_H
#define FONTCOMBOBOX_H

#include "combobox.h"

class FontComboBox : public ComboBox
{
    Q_OBJECT
public:
    explicit FontComboBox(QWidget *parent = 0);
    ~FontComboBox();

    const QFont& value() const;
    void setValue(const QFont& f, bool spontaneous = true);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    QFont font;

signals:
    void valueChanged(QFont);

public slots:
};

#endif // FONTCOMBOBOX_H
