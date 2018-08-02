#include "mediapanel.h"
#include "ui_mediapanel.h"
#include "session.h"
#include "ui_projectsettings.h"

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

    timer.setInterval(1000 / session->projectSettings->getFPS());

    connect(&timer, &QTimer::timeout, [this]() {
        session->setDocumentTime(session->projectSettings->getDocumentTime() + direction * (time.restart() * session->projectSettings->getFPS() + 500) / 1000);
    });

    connect(session->projectSettings->ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        timer.setInterval(1000 / value);
    });
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
