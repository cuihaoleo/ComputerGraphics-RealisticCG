#include "realisticscene.h"

#include <cmath>
#include <QMatrix4x4>

RealisticScene::RealisticScene(const RScene &sence, const QSize &imageSize, QObject *parent):
    QGraphicsScene(parent)
{
    this->setBackgroundBrush(Qt::gray);
    this->world = sence;
    this->imageSize = imageSize;
    this->center = QPointF(imageSize.width()/2.0, imageSize.height()/2.0);
    this->sphereRadius = qMin(imageSize.width()/2.0, imageSize.height()/2.0);
    this->image = QImage(imageSize, QImage::Format_RGB32);
    this->pixmapItem = addPixmap(QPixmap::fromImage(this->image));

    viewportSize = QSizeF(5, 5);
    pixmapItem->setPixmap(QPixmap::fromImage(image));
}

void RealisticScene::setView(QVector3D viewPoint, double angle)
{
    view = RView(viewPoint, angle);
    RDepthBuffer buffer = view.lookAt(world, imageSize);
    buffer.toImage(image, viewportSize);
    pixmapItem->setPixmap(QPixmap::fromImage(image));
}

void RealisticScene::setView(QVector3D viewPoint, QVector3D viewUp)
{
    view = RView(viewPoint, viewUp);
    RDepthBuffer buffer = view.lookAt(world, imageSize);
    buffer.toImage(image, viewportSize);
    pixmapItem->setPixmap(QPixmap::fromImage(image));
}
