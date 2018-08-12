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

    connect(ui->horizontalSlider, &QAbstractSlider::sliderPressed, this, &MediaPanel::pause);
    connect(ui->horizontalSlider, &QAbstractSlider::sliderReleased, this, &MediaPanel::resume);
}

MediaPanel::~MediaPanel()
{
    delete ui;
}

void MediaPanel::recordActiveObjects()
{
    ui->horizontalSlider->update();
}

void MediaPanel::autokeying(bool checked)
{
    ui->record->setChecked(checked);
}

void MediaPanel::playForwards(bool checked)
{
    if (checked)
    {
        if (ui->reverse->isChecked())
            ui->reverse->click();

        resume();
    }
    else
        pause();

    ui->play->setChecked(checked);
}

void MediaPanel::playBackwards(bool checked)
{
    if (checked)
    {
        if (ui->play->isChecked())
            ui->play->click();

        resume();
    }
    else
        pause();

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

void MediaPanel::pause()
{
    timer.stop();
}

void MediaPanel::resume()
{
    if (session->playForwards || session->playBackwards)
    {
        time.restart();
        timer.start();
    }
}

constexpr int rem(int a, int b) {
    return (a % b + b) % b;
}

void MediaPanel::step()
{
    int delta = (time.restart() * session->projectSettings->getFPS() + 500) / 1000;
    int time = session->projectSettings->getDocumentTime();
    int next = time + (session->playForwards ? delta : -delta);

    if (session->playForwards)
    {
        int maxTime = session->previewRange ? session->projectSettings->getPreviewMaxTime() : session->projectSettings->getMaximumTime();

        if (next <= maxTime)
            session->setDocumentTime(next);
        else
        {
            if (maxTime == ui->spinBox->maximum())
            {
                bool ok;
                session->simulation->getFrame(next, &ok);

                if (ok)
                    return session->setDocumentTime(next);
                else
                    maxTime = session->simulation->getFrameCount() - 1;
            }

            int minTime = session->previewRange ? session->projectSettings->getPreviewMinTime() : session->projectSettings->getMinimumTime();
            int frameCount = maxTime - minTime + 1;

            switch (session->playMode)
            {
            case Session::PM_Simple:
                session->setDocumentTime(maxTime);
                ui->play->click();
                break;
            case Session::PM_Cycle:
                session->setDocumentTime(minTime + rem(next - minTime, frameCount));
                break;
            case Session::PM_PingPong:
                next = rem(next - minTime, 2 * frameCount - 2);
                if (next >= frameCount)
                {
                    session->setDocumentTime(minTime + 2 * frameCount - 2 - next);
                    ui->reverse->click();
                }
                else
                    session->setDocumentTime(minTime + next);
                break;
            }
        }
    }
    else
    {
        int minTime = session->previewRange ? session->projectSettings->getPreviewMinTime() : session->projectSettings->getMinimumTime();

        if (next >= minTime)
            session->setDocumentTime(next);
        else
        {
            int maxTime = session->previewRange ? session->projectSettings->getPreviewMaxTime() : session->projectSettings->getMaximumTime();
            int frameCount = maxTime - minTime + 1;

            switch (session->playMode)
            {
            case Session::PM_Simple:
                session->setDocumentTime(minTime);
                ui->reverse->click();
                break;
            case Session::PM_Cycle:
                session->setDocumentTime(minTime + rem(next - minTime, frameCount));
                break;
            case Session::PM_PingPong:
                next = rem(next - minTime, 2 * frameCount - 2);
                if (next >= frameCount)
                {
                    session->setDocumentTime(minTime + 2 * frameCount - 2 - next);
                    ui->play->click();
                }
                else
                    session->setDocumentTime(minTime + next);
                break;
            }
        }
    }
}
