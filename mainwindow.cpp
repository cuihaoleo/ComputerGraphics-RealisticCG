#define _USE_MATH_DEFINES

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "rscene.h"
#include "realisticscene.h"

#include <QDebug>

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

    realisticScene = new RealisticScene(world, QSize(512, 512));
    ui->graphicsView->setScene(realisticScene);
    ui->graphicsView->fitInView(realisticScene->sceneRect(), Qt::KeepAspectRatio);
    ui->graphicsView->show();

    ui->sliderPolar->setValue(polarStep);
    ui->sliderAzimuth->setValue(azimuthStep);

    QWidget::showEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sliderPolar_valueChanged(int value)
{
    this->polarStep = value;
    updateScene();
}

void MainWindow::on_sliderAzimuth_valueChanged(int value)
{
    this->azimuthStep = value;
    updateScene();
}

void MainWindow::updateScene()
{
    using namespace std;

    double polar = polarStep / 100.0 * M_PI;
    double azimuth = azimuthStep / 100.0 * M_PI * 2;

    qDebug() << "Polar:" << polar << ", Azimuth:" << azimuth;

    double z = -radius * sin(polar) * cos(azimuth);
    double x = radius * sin(polar) * sin(azimuth);
    double y = radius * cos(polar);

    realisticScene->setView(QVector3D(x, y, z), 0);
}
