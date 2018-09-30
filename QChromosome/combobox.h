#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>

class ComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit ComboBox(QWidget *parent = 0);
    ~ComboBox();

    void setMultipleValues();

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void paintEvent(QPaintEvent *);

signals:

public slots:
    void setCurrentIndex(int index, bool spontaneous = true);
};

#endif // COMBOBOX_H
