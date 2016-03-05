#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../QtChromosomeViz_v2/bartekm_code/NullSimulation.h"
#include "../QtChromosomeViz_v2/SelectionOperationsWidget.hpp"//TODO do wywalenia po zaimplementowaniu widgeta

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    simulation(std::make_shared<NullSimulation>()),
    currentFrame(0),//TODO być może wywalić, jak ukryje się suwaki, gdy jest plik jednoklatkowy
    lastFrame(0),//TODO być może wywalić, jak ukryje się suwaki, gdy jest plik jednoklatkowy
    actionGroup(new QActionGroup(this))
{
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    ui->setupUi(this);

    ui->tabWidget->hide();

    actionGroup->addAction(ui->actionSelect);
    actionGroup->addAction(ui->actionMove);
    actionGroup->addAction(ui->actionRotate);
    actionGroup->addAction(ui->actionScale);

    bindings.insert(Qt::Key_Q, ui->actionMove);
    bindings.insert(Qt::Key_W, ui->actionRotate);
    bindings.insert(Qt::Key_E, ui->actionScale);

    modifiers.push_back(ui->actionMove);

    cacheProperties(this);

    //TODO do wywalenia po zaimplementowaniu widgeta
    auto x = new SelectionOperationsWidget(ui->tab);
    x->setVizWidget(ui->scene);
    x->setStyleSheet("SelectionOperationsWidget>QLabel { color: #d9d9d9; }");
}

MainWindow::~MainWindow()
{
    delete ui;
}

#include "draggable.h"

typedef QList<QPair<QByteArray, QVariant> > QPropertyStyleSheet;

static QHash<QPair<QObject*, QEvent::Type>, QPair<QPropertyStyleSheet, QPropertyStyleSheet*> > qproperty_enter;
static QHash<QPair<QObject*, QEvent::Type>, QPropertyStyleSheet> qproperty_leave;

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (Draggable::pressedButton() != Qt::NoButton)
    {
        if (event->type() == QEvent::MouseButtonPress && event->spontaneous())
            return true;

        if (event->type() == QEvent::MouseButtonRelease && reinterpret_cast<QMouseEvent*>(event)->button() != Draggable::pressedButton())
            return true;
    }

    auto enterEvent = qproperty_enter.find(QPair<QObject*, QEvent::Type>(watched, event->type()));

    if (enterEvent != qproperty_enter.end())
    {
        auto *backup = enterEvent.value().second;

        for (const auto& i : enterEvent.value().first)
        {
            backup->push_back({ i.first, watched->property(i.first) });
            watched->setProperty(i.first, i.second);
        }
    }

    auto leaveEvent = qproperty_leave.find(QPair<QObject*, QEvent::Type>(watched, event->type()));

    if (leaveEvent != qproperty_leave.end())
    {
        for (const auto& i : leaveEvent.value())
            watched->setProperty(i.first, i.second);

        leaveEvent.value().clear();
    }

    return QMainWindow::eventFilter(watched, event);
}

#include "../QtChromosomeViz_v2/bartekm_code/PDBSimulation.h"

void MainWindow::openSimulation()
{
    QString path = QFileDialog::getOpenFileName(this, "", "/home", "RCSB Protein Data Bank (*.pdb)");

    if (!path.isEmpty())
    {//TODO tu może być problem z synchronizacją i gubieniem sygnału
        QObject::disconnect(this, SLOT(updateFrameCount(int)));

        simulation = std::make_shared<PDBSimulation>(path.toStdString());

        ui->scene->setSimulation(simulation);
        ui->plot->setSimulation(simulation);

        ui->horizontalSlider->setMaximum(0);
        ui->horizontalSlider->setValue(0);

        ui->spinBox->setMaximum(0);
        ui->spinBox->setValue(0);

        lastFrame = 0;

        connect(simulation.get(), SIGNAL(frameCountChanged(int)), this, SLOT(updateFrameCount(int)));
        simulation->getFrame(10);//TODO paskudny hack, usunąć po dodaniu wątku
    }
}

void MainWindow::updateFrameCount(int n)
{
    lastFrame = n - 1;

    ui->horizontalSlider->setMaximum(lastFrame);
    ui->spinBox->setMaximum(lastFrame);
    ui->plot->setMaximum(lastFrame);
}

void MainWindow::setFrame(int n)
{
    Q_ASSERT(0 <= n && n <= lastFrame);

    currentFrame = n;

    ui->horizontalSlider->setValue(n);
    ui->spinBox->setValue(n);
    ui->scene->setFrame(n);
    ui->plot->setFrame(n);
}

void MainWindow::start()
{
    setFrame(0);
}

void MainWindow::previous()
{
    if (currentFrame != 0)
        setFrame(--currentFrame);
    else
        if (ui->reverse->isChecked())
            ui->reverse->click();
}

void MainWindow::reverse(bool checked)
{
    if (checked)
    {
        if (ui->play->isChecked())
            ui->play->click();

        connect(&timer, SIGNAL(timeout()), this, SLOT(previous()));
        timer.start();
    }
    else
    {
        timer.stop();
        timer.disconnect();
    }
}

void MainWindow::play(bool checked)
{
    if (checked)
    {
        if (ui->reverse->isChecked())
            ui->reverse->click();

        connect(&timer, SIGNAL(timeout()), this, SLOT(next()));
        timer.start();
    }
    else
    {
        timer.stop();
        timer.disconnect();
    }
}

void MainWindow::next()
{
    simulation->getFrame(currentFrame+1);//TODO paskudny hack, usunąć po dodaniu wątku
    if (currentFrame != lastFrame)
        setFrame(++currentFrame);
    else
        if (ui->play->isChecked())
            ui->play->click();
}

void MainWindow::end()
{
    setFrame(lastFrame);
    simulation->getFrame(lastFrame+1);//TODO paskudny hack, usunąć po dodaniu wątku
}

void MainWindow::selectAll()
{
    QList<unsigned int> all;

    for (unsigned int i = 0; i < simulation->getFrame(currentFrame)->atoms.size(); i++)
        all.push_back(i);

    //ui->scene->setVisibleSelection(all);
}

void MainWindow::handleSelection(const AtomSelection &selection)
{
    ui->camera->setOrigin(selection.weightCenter());

    if (selection.atomCount())
        ui->tabWidget->show();
    else
        ui->tabWidget->hide();
}

void MainWindow::setBaseAction(bool enabled)
{
    if (enabled)
        modifiers.last() = qobject_cast<QAction*>(sender());
}

#include <QKeyEvent>

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return event->ignore();

    auto i = bindings.find(event->key());

    if (i != bindings.end())
    {
        lookup[i.key()] = modifiers.insert(modifiers.end(), i.value());
        actionGroup->setDisabled(true);
        i.value()->setChecked(true);
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return event->ignore();

    auto i = lookup.find(event->key());

    if (i != lookup.end())
    {
        modifiers.erase(i.value());
        modifiers.last()->setChecked(true);

        lookup.erase(i);

        if (lookup.isEmpty())
            actionGroup->setEnabled(true);
    }

    QMainWindow::keyReleaseEvent(event);
}

static const QHash<QString, QPair<QEvent::Type, QEvent::Type> > pseudostate2event = { { "hover", { QEvent::HoverEnter, QEvent::HoverLeave } }, { "pressed", { QEvent::MouseButtonPress, QEvent::MouseButtonRelease } } };

void MainWindow::cacheProperties(QWidget *widget, QHash<QString, QHash<QString, QHash<QString, QVariant> > > cache)
{
    static const QRegularExpression selector("(?<class>\\w*)\\s*:(?<pseudostate>\\w+)\\s*{(?<stylesheet>[^{}]*)}");

    auto i = selector.globalMatch(widget->styleSheet());

    while (i.hasNext())
    {
        auto selector = i.next();

        auto& styleSheet = cache[selector.captured("class")][selector.captured("pseudostate")];

        static const QRegularExpression property("qproperty-(?<name>\\w+)\\s*:\\s*(?<value>\\S+)\\s*;");

        auto i = property.globalMatch(selector.captured("stylesheet"));

        while (i.hasNext())
        {
            auto property = i.next();

            styleSheet[property.captured("name")] = property.captured("value");
        }
    }

    auto styleSheet = cache[widget->metaObject()->className()];

    for (auto i = cache[""].cbegin(); i != cache[""].cend(); i++)
    {
        for (auto j = i.value().cbegin(); j != i.value().cend(); j++)
        {
            styleSheet[i.key()].insert(j.key(), j.value());
        }
    }

    cache.remove("");

    for (auto i = styleSheet.cbegin(); i != styleSheet.cend(); i++)
    {
        auto& pseudostate = qproperty_enter[QPair<QObject*, QEvent::Type>(widget, pseudostate2event[i.key()].first)];

        for (auto j = i.value().cbegin(); j != i.value().cend(); j++)
            pseudostate.first.push_back({ j.key().toUtf8(), j.value() });

        pseudostate.second = &qproperty_leave[QPair<QObject*, QEvent::Type>(widget, pseudostate2event[i.key()].second)];
    }

    for (auto child : widget->children())
        if (auto tmp = qobject_cast<QWidget*>(child))
            cacheProperties(tmp, cache);
}
