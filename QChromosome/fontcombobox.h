#ifndef FONTCOMBOBOX_H
#define FONTCOMBOBOX_H

#include <QStyledItemDelegate>

class QFontFamilyStyledDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit QFontFamilyStyledDelegate(QObject *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#include <QFontComboBox>

class FontComboBox : public QFontComboBox
{
    Q_OBJECT
public:
    explicit FontComboBox(QWidget *parent = 0);
    ~FontComboBox();

    void setMultipleValues();

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void paintEvent(QPaintEvent *);

signals:

public slots:
    void setCurrentIndex(int index, bool spontaneous = true);

private:
    bool multiple;
};

#endif // FONTCOMBOBOX_H
