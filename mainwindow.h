#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "realisticscene.h"

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
    void on_sliderAzimuth_valueChanged(int value);

    void on_sliderPolar_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    RealisticScene *realisticScene;

    int polarStep = 50;
    int azimuthStep = 0;
    double radius = 10.0;

    void updateScene();
};

#endif // MAINWINDOW_H
