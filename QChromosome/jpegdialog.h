#ifndef JPEGDIALOG_H
#define JPEGDIALOG_H

#include <QDialog>

namespace Ui {
class JPEGDialog;
}

class JPEGDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JPEGDialog(QWidget *parent = nullptr);
    ~JPEGDialog();

    void setQuality(int q);
    int getQuality() const;

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

private:
    Ui::JPEGDialog *ui;

    int quality;
};

#endif // JPEGDIALOG_H
