#include "mediapanel.h"
#include "ui_mediapanel.h"
#include "session.h"
#include "ui_mainwindow.h"

MediaPanel::MediaPanel(Session* s, Ui::MainWindow *ui__, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MediaPanel),
    ui_(ui__),
    session(s)
{
    ui->setupUi(this);

    connect(ui->play, &QPushButton::clicked, this, &MediaPanel::playForwards);
    connect(ui->reverse, &QPushButton::clicked, this, &MediaPanel::playBackwards);
    connect(ui->start, &QPushButton::clicked, this, &MediaPanel::goToStart);
    connect(ui->end, &QPushButton::clicked, this, &MediaPanel::goToEnd);
    connect(ui->next, &QPushButton::clicked, this, &MediaPanel::goToNextFrame);
    connect(ui->previous, &QPushButton::clicked, this, &MediaPanel::goToPreviousFrame);
    connect(ui->key, &QPushButton::clicked, this, &MediaPanel::recordActiveObjects);
    connect(ui->record, &QPushButton::clicked, this, &MediaPanel::autokeying);

    connect(ui_->actionPlay_forwards, &QAction::triggered, this, &MediaPanel::playForwards);
    connect(ui_->actionPlay_backwards, &QAction::triggered, this, &MediaPanel::playBackwards);
    connect(ui_->actionGo_to_start, &QAction::triggered, this, &MediaPanel::goToStart);
    connect(ui_->actionGo_to_end, &QAction::triggered, this, &MediaPanel::goToEnd);
    connect(ui_->actionGo_to_next_frame, &QAction::triggered, this, &MediaPanel::goToNextFrame);
    connect(ui_->actionGo_to_previous_frame, &QAction::triggered, this, &MediaPanel::goToPreviousFrame);
    connect(ui_->actionRecord_Active_Objects, &QAction::triggered, this, &MediaPanel::recordActiveObjects);
    connect(ui_->actionAutokeying, &QAction::triggered, this, &MediaPanel::autokeying);

    timer.setInterval(1000 / session->projectSettings->getFPS());

    connect(&timer, &QTimer::timeout, [this]() {
        session->setDocumentTime(session->projectSettings->getDocumentTime() + direction * (time.restart() * session->projectSettings->getFPS() + 500) / 1000);
    });

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
    ui_->actionAutokeying->setChecked(checked);
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
    ui_->actionPlay_forwards->setChecked(checked);
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
    ui_->actionPlay_backwards->setChecked(checked);
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
