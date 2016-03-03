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

#include <QMouseEvent>
#include "draggable.h"

static const QHash<QEvent::Type, QString> enterEvents = { { QEvent::HoverEnter, "hover" }, { QEvent::MouseButtonPress, "pressed" } };
static const QHash<QEvent::Type, QString> leaveEvents = { { QEvent::HoverLeave, "hover" }, { QEvent::MouseButtonRelease, "pressed" } };

typedef QList<QPair<QString, QVariant> > QPropertyStyle;

Q_DECLARE_METATYPE(QPropertyStyle)

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (Draggable::pressedButton() != Qt::NoButton)
    {
        if (event->type() == QEvent::MouseButtonPress && event->spontaneous())
            return true;

        if (event->type() == QEvent::MouseButtonRelease && reinterpret_cast<QMouseEvent*>(event)->button() != Draggable::pressedButton())
            return true;
    }

    auto pseudostate = enterEvents.find(event->type());

    if (pseudostate != enterEvents.end())
    {
        const QVariant& variant = watched->property(("_qproperty_" + pseudostate.value() + "_style").toUtf8().constData());

        if (variant.isValid())
        {
            QPropertyStyle backup;

            for (auto& i : variant.value<QPropertyStyle>())
            {
                backup.push_back({i.first, watched->property(i.first.toUtf8().constData())});

                watched->setProperty(i.first.toUtf8().constData(), i.second);
            }

            watched->setProperty(("_qproperty_" + pseudostate.value() + "_backup").toUtf8().constData(), QVariant::fromValue<QPropertyStyle>(backup));
        }
    }

    pseudostate = leaveEvents.find(event->type());

    if (pseudostate != leaveEvents.end())
    {
        const QVariant& variant = watched->property(("_qproperty_" + pseudostate.value() + "_backup").toUtf8().constData());

        if (variant.isValid())
        {
            for (auto& i : variant.value<QPropertyStyle>())
                watched->setProperty(i.first.toUtf8().constData(), i.second);

            watched->setProperty(("_qproperty_" + pseudostate.value() + "_backup").toUtf8().constData(), QVariant());
        }
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

void MainWindow::cacheProperties(QWidget *widget, QHash<QString, QHash<QString, QHash<QString, QVariant> > > cache)
{
    static const QRegularExpression styleRegExp("(?<class>\\w*)\\s*:(?<pseudostate>\\w+)\\s*{(?<stylesheet>[^{}]*)}");
    QRegularExpressionMatchIterator i = styleRegExp.globalMatch(widget->styleSheet());

    while (i.hasNext())
    {
        QRegularExpressionMatch style = i.next();

        auto& stylesheet = cache[style.captured("class")][style.captured("pseudostate")];

        static const QRegularExpression propertyRegExp("qproperty-(?<name>\\w+)\\s*:\\s*(?<value>\\S+)\\s*;");
        QRegularExpressionMatchIterator j = propertyRegExp.globalMatch(style.captured("stylesheet"));

        while (j.hasNext())
        {
            QRegularExpressionMatch property = j.next();

            stylesheet[property.captured("name")] = property.captured("value");
        }
    }

    QHash<QString, QHash<QString, QVariant> > styleSheet = cache[widget->metaObject()->className()];

    for (auto i = cache[""].cbegin(); i != cache[""].cend(); i++)
        for (auto j = i.value().cbegin(); j != i.value().cend(); j++)
            styleSheet[i.key()][j.key()] = j.value();

    cache.remove("");

    for (auto i = styleSheet.cbegin(); i != styleSheet.cend(); i++)
    {
        QPropertyStyle style;

        for (auto j = i.value().cbegin(); j != i.value().cend(); j++)
            style.push_back({j.key(), j.value()});

        widget->setProperty(("_qproperty_" + i.key() + "_style").toUtf8().constData(), QVariant::fromValue(style));
    }

    for (auto child : widget->children())
        if (auto tmp = qobject_cast<QWidget*>(child))
            cacheProperties(tmp, cache);
}
