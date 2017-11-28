#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "rscene.h"
#include "realisticscene.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void MainWindow::showEvent(QShowEvent* event) {
    RScene world;
    world.addPoint(QVector3D(0, 1, 0));
    world.addPoint(QVector3D(0, -1, 1));
    world.addPoint(QVector3D(1, -1, -1));
    world.addPoint(QVector3D(-1, -1, -1));
    world.addTriangle(0, 1, 2);
    world.addTriangle(0, 2, 3);
    world.addTriangle(0, 1, 3);
    world.addTriangle(1, 2, 3);

    RealisticScene* scene = new RealisticScene(world, QSize(512, 512));
    ui->graphicsView->setScene(scene);
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    ui->graphicsView->show();

    QWidget::showEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}
