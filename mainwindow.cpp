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
    int a, b, c, d, e, f, g, h;

    a = world.addPoint(QVector3D(0, 1, 0));
    b = world.addPoint(QVector3D(0, -1, 1));
    c = world.addPoint(QVector3D(1, -1, -1));
    d = world.addPoint(QVector3D(-1, -1, -1));
    world.addTriangle(a, b, c, QVector3D(0.8, 1.0, 1.0));
    world.addTriangle(a, c, d, QVector3D(1.0, 0.8, 1.0));
    world.addTriangle(a, b, d, QVector3D(1.0, 1.0, 0.8));
    world.addTriangle(b, c, d);

    a = world.addPoint(QVector3D(1.2, 1.2, 0.8));
    b = world.addPoint(QVector3D(1.2, 0.8, 0.8));
    c = world.addPoint(QVector3D(0.8, 0.8, 0.8));
    d = world.addPoint(QVector3D(0.8, 1.2, 0.8));
    e = world.addPoint(QVector3D(1.2, 1.2, 1.2));
    f = world.addPoint(QVector3D(1.2, 0.8, 1.2));
    g = world.addPoint(QVector3D(0.8, 0.8, 1.2));
    h = world.addPoint(QVector3D(0.8, 1.2, 1.2));

    world.addPolygon({a, b, c, d}, 1.0);
    world.addPolygon({e, f, g, h}, 1.0);
    world.addPolygon({a, b, f, e}, 1.0);
    world.addPolygon({a, d, h, e}, 1.0);
    world.addPolygon({b, c, g, f}, 1.0);
    world.addPolygon({c, d, h, g}, 1.0);

    a = world.addPoint(QVector3D(2, -1.5, 2));
    b = world.addPoint(QVector3D(-2, -1.5, 2));
    c = world.addPoint(QVector3D(-2, -1.5, -2));
    d = world.addPoint(QVector3D(2, -1.5, -2));
    world.addPolygon({a, b, c, d}, 0.4);

    a = world.addPoint(QVector3D(2, 1.5, -2));
    b = world.addPoint(QVector3D(-2, 1.5, -2));
    c = world.addPoint(QVector3D(-2, -1.5, -2));
    d = world.addPoint(QVector3D(2, -1.5, -2));
    world.addPolygon({a, b, c, d}, 0.4);

    world.setBaseLight(0.2);
    world.addLight(QVector3D(1, 4, 5), 5);

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
    viewAzimuth = value / 100.0 * M_PI + M_PI_2;
    updateScene();
}

void MainWindow::on_sliderY_valueChanged(int value)
{
    qDebug() << Q_FUNC_INFO << value;
    viewY = -0.5 + value / 20.0;
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
