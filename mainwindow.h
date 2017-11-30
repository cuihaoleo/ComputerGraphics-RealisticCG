#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define _USE_MATH_DEFINES

#include "rscene.h"
#include "rview.h"

#include <QMainWindow>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QSize>
#include <QSizeF>
#include <QVector3D>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void showEvent(QShowEvent* event);

private slots:
    void on_sliderY_valueChanged(int value);
    void on_sliderAzimuth_valueChanged(int value);
    void on_sliderLightAzimuth_valueChanged(int value);
    void on_sliderLightB_valueChanged(int value);
    void on_sliderLightG_valueChanged(int value);
    void on_sliderLightR_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    QGraphicsPixmapItem *pixmapItem;
    QGraphicsScene *canvas;
    QImage image;
    QSize imageSize;
    QSizeF viewportSize;
    QVector3D lightColor;

    RScene world;
    RView view;

    bool blockUpdate;

    double viewAzimuth;
    double viewY;
    double viewRadius = 8.0;

    double lightAzimuth;
    double lightY = 4.0;
    double lightRadius = 6.0;
    double brightness = 5.0;

    void updateLight();
    void updateView();
};

#endif // MAINWINDOW_H
