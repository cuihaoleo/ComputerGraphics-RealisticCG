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

    world.addPoint(QVector3D(3, -1.5, 3));
    world.addPoint(QVector3D(-3, -1.5, 3));
    world.addPoint(QVector3D(-3, -1.5, -3));
    world.addPoint(QVector3D(3, -1.5, -3));
    world.addTriangle(4, 5, 6, 0.2);
    world.addTriangle(6, 7, 4, 0.2);

    world.setBaseLight(0.2);
    world.addLight(QVector3D(0, 10, 10), 20);

    realisticScene = new RealisticScene(world, QSize(512, 512));
    ui->graphicsView->setScene(realisticScene);
    ui->graphicsView->fitInView(realisticScene->sceneRect(), Qt::KeepAspectRatio);
    ui->graphicsView->show();

    ui->sliderAzimuth->setValue(58);
    ui->sliderY->setValue(26);

    QWidget::showEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sliderAzimuth_valueChanged(int value)
{
    qDebug() << Q_FUNC_INFO << value;
    viewAzimuth = value / 100.0 * 2.0 * M_PI;
    updateScene();
}

void MainWindow::on_sliderY_valueChanged(int value)
{
    qDebug() << Q_FUNC_INFO << value;
    viewY = 1.0 + value / 50.0;
    updateScene();
}

void MainWindow::updateScene()
{
    using namespace std;

    static const QVector3D axisY(0, 1, 0);
    double rxz = std::sqrt(viewRadius * viewRadius - viewY * viewY);
    assert(rxz > 0);
    double viewZ = -rxz * cos(viewAzimuth);
    double viewX = rxz * sin(viewAzimuth);
    QVector3D viewPoint = QVector3D(viewX, viewY, viewZ);

    QVector3D viewUp;
    viewUp = QVector3D::crossProduct(viewPoint, axisY);
    viewUp = QVector3D::crossProduct(viewPoint, viewUp);

    realisticScene->setView(QVector3D(viewX, viewY, viewZ), viewUp);
}
