#include "mediapanel.h"
#include "ui_mediapanel.h"
#include "session.h"

MediaPanel::MediaPanel(Session* s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MediaPanel),
    session(s)
{
    ui->setupUi(this);

    connect(ui->play, &QPushButton::clicked, this, &MediaPanel::playForwards);
    connect(ui->reverse, &QPushButton::clicked, this, &MediaPanel::playBackwards);
    connect(ui->start, &QPushButton::clicked, this, &MediaPanel::goToStart);
    connect(ui->end, &QPushButton::clicked, this, &MediaPanel::goToEnd);
    connect(ui->next, &QPushButton::clicked, this, &MediaPanel::goToNextFrame);
    connect(ui->previous, &QPushButton::clicked, this, &MediaPanel::goToPreviousFrame);

    connect(ui->key, &QPushButton::clicked, [this] {
        session->currentCamera->captureFrame();
        ui->horizontalSlider->update();
    });

    connect(ui->record, &QPushButton::toggled, [this](bool checked) {
        Camera::setAutomaticKeyframing(checked);
    });

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
