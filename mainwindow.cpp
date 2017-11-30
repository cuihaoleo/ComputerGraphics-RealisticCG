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

    viewportSize = QSizeF(6, 4);
    imageSize = QSize(600, 400);

    canvas = new QGraphicsScene();
    canvas->setBackgroundBrush(Qt::gray);
    image = QImage(imageSize, QImage::Format_RGB32);
    pixmapItem = canvas->addPixmap(QPixmap::fromImage(image));

    ui->graphicsView->setScene(canvas);
    ui->graphicsView->fitInView(canvas->sceneRect(), Qt::KeepAspectRatio);
    ui->graphicsView->show();

    blockUpdate = true;
    ui->sliderAzimuth->setValue(58);
    ui->sliderY->setValue(26);
    ui->sliderLightAzimuth->setValue(48);
    ui->sliderLightB->setValue(50);
    ui->sliderLightG->setValue(50);
    ui->sliderLightR->setValue(50);
    blockUpdate = false;
    updateLight();

    QWidget::showEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sliderAzimuth_valueChanged(int value)
{
    viewAzimuth = value / 100.0 * M_PI + M_PI_2;
    updateView();
}

void MainWindow::on_sliderY_valueChanged(int value)
{
    viewY = -0.5 + value / 20.0;
    updateView();
}

void MainWindow::on_sliderLightAzimuth_valueChanged(int value)
{
    lightAzimuth = value / 100.0 * M_PI * 2;
    updateLight();
}

void MainWindow::on_sliderLightB_valueChanged(int value)
{
    lightColor[0] = value / 10.0;
    updateLight();
}

void MainWindow::on_sliderLightG_valueChanged(int value)
{
    lightColor[1] = value / 10.0;
    updateLight();
}

void MainWindow::on_sliderLightR_valueChanged(int value)
{
    lightColor[2] = value / 10.0;
    updateLight();
}

void MainWindow::updateView()
{
    using namespace std;

    if (blockUpdate) return;

    static const QVector3D axisY(0, 1, 0);
    double rxz = sqrt(viewRadius * viewRadius - viewY * viewY);
    double viewZ = -rxz * cos(viewAzimuth);
    double viewX = rxz * sin(viewAzimuth);
    QVector3D viewPoint = QVector3D(viewX, viewY, viewZ);

    QVector3D viewUp;
    viewUp = QVector3D::crossProduct(viewPoint, axisY);
    viewUp = QVector3D::crossProduct(viewPoint, viewUp);

    view = RView(QVector3D(viewX, viewY, viewZ), viewUp);
    RDepthBuffer buffer = view.lookAt(world, imageSize);
    buffer.toImage(image, viewportSize);
    pixmapItem->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::updateLight()
{
    using namespace std;

    if (blockUpdate) return;

    double rxz = sqrt(lightRadius * lightRadius - lightY * lightY);
    double lightZ = -rxz * cos(lightAzimuth);
    double lightX = rxz * sin(lightAzimuth);

    world.clearLights();
    world.addLight(QVector3D(lightX, lightY, lightZ), lightColor);

    updateView();
}
