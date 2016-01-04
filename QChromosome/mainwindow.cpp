#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->move->load(":/move");
    ui->rotate->load(":/rotate");
    ui->scale->load(":/scale");
}

MainWindow::~MainWindow()
{
    delete ui;
}
