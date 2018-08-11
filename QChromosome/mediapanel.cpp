#include "mediapanel.h"
#include "ui_mediapanel.h"
#include "session.h"
#include "mainwindow.h"

MediaPanel::MediaPanel(Session* s, MainWindow* w, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MediaPanel),
    session(s)
{
    ui->setupUi(this);

    connect(ui->play, &QPushButton::clicked, w, &MainWindow::playForwards);
    connect(ui->reverse, &QPushButton::clicked, w, &MainWindow::playBackwards);
    connect(ui->start, &QPushButton::clicked, w, &MainWindow::goToStart);
    connect(ui->end, &QPushButton::clicked, w, &MainWindow::goToEnd);
    connect(ui->next, &QPushButton::clicked, w, &MainWindow::goToNextFrame);
    connect(ui->previous, &QPushButton::clicked, w, &MainWindow::goToPreviousFrame);
    connect(ui->key, &QPushButton::clicked, w, &MainWindow::recordActiveObjects);
    connect(ui->record, &QPushButton::clicked, w, &MainWindow::autokeying);

    timer.setInterval(1000 / session->projectSettings->getFPS());
    connect(&timer, &QTimer::timeout, this, &MediaPanel::step);

    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), session, &Session::setDocumentTime);
    connect(ui->horizontalSlider, &QSlider::valueChanged, session, &Session::setDocumentTime);
    connect(ui->spinBox_2, QOverload<int>::of(&QSpinBox::valueChanged), session, &Session::setMinimumTime);
    connect(ui->spinBox_3, QOverload<int>::of(&QSpinBox::valueChanged), session, &Session::setMaximumTime);
    connect(ui->horizontalSlider_2, &RangeSlider::lowerBoundChanged, session, &Session::setPreviewMinTime);
    connect(ui->horizontalSlider_2, &RangeSlider::upperBoundChanged, session, &Session::setPreviewMaxTime);
}

MediaPanel::~MediaPanel()
{
    delete ui;
}

void MediaPanel::recordActiveObjects()
{
    session->currentCamera->captureFrame();
    ui->horizontalSlider->update();
}

void MediaPanel::autokeying(bool checked)
{
    session->setAutomaticKeyframing(checked);
    ui->record->setChecked(checked);
}

void MediaPanel::playForwards(bool checked)
{
    if (checked)
    {
        if (ui->reverse->isChecked())
            ui->reverse->click();

        direction = +1;
        time.restart();

        timer.start();
    }
    else
        timer.stop();

    ui->play->setChecked(checked);
}

void MediaPanel::playBackwards(bool checked)
{
    if (checked)
    {
        if (ui->play->isChecked())
            ui->play->click();

        direction = -1;
        time.restart();

        timer.start();
    }
    else
        timer.stop();

    ui->reverse->setChecked(checked);
}

void MediaPanel::goToStart()
{
    session->setDocumentTime(session->projectSettings->getMinimumTime());
}

void MediaPanel::goToEnd()
{
    session->setDocumentTime(session->projectSettings->getMaximumTime());
}

void MediaPanel::goToNextFrame()
{
    session->setDocumentTime(session->projectSettings->getDocumentTime() + 1);
}

void MediaPanel::goToPreviousFrame()
{
    session->setDocumentTime(session->projectSettings->getDocumentTime() - 1);
}

void MediaPanel::setFPS(int fps)
{
    timer.setInterval(1000 / fps);
}

void MediaPanel::setDocumentTime(int time)
{
    ui->spinBox->setValue(time, false);
    ui->horizontalSlider->setValue(time, false);
}

void MediaPanel::setMinimumTime(int time)
{
    ui->spinBox_2->setValue(time, false);
    ui->spinBox_3->setMinimum(time + 1);
    ui->horizontalSlider_2->setMinimum(time);
}

void MediaPanel::setMaximumTime(int time)
{
    ui->spinBox_3->setValue(time, false);
    ui->spinBox_2->setMaximum(time > 0 ? time - 1 : 0);
    ui->horizontalSlider_2->setMaximum(time);
}

void MediaPanel::setPreviewMinTime(int time)
{
    ui->horizontalSlider_2->setLowerBound(time, false);
    ui->horizontalSlider->setSoftMinimum(time);
}

void MediaPanel::setPreviewMaxTime(int time)
{
    ui->horizontalSlider_2->setUpperBound(time, false);
    ui->horizontalSlider->setSoftMaximum(time);
}

void MediaPanel::setLastFrame(int time)
{
    ui->spinBox->setMaximum(time);
    ui->horizontalSlider->setMaximum(time);
    ui->spinBox_3->setMaximum(time);
}

void MediaPanel::changeCamera(Camera* camera)
{
    ui->horizontalSlider->setSplineInterpolator(camera);
}

void MediaPanel::step()
{
    int delta = (time.restart() * session->projectSettings->getFPS() + 500) / 1000;
    int time = session->projectSettings->getDocumentTime();
    int next = time + direction * delta;

    if (direction == 1)
    {
        bool ok;
        session->simulation->getFrame(next, &ok);

        if (ok)
            session->setDocumentTime(next);
        else
        {
            session->setDocumentTime(session->simulation->getFrameCount() - 1);
            ui->play->click();
        }
    }
    else
    {
        if (next > 0)
            session->setDocumentTime(next);
        else
        {
            session->setDocumentTime(0);
            ui->reverse->click();
        }
    }
}
