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
    this->image = QImage(imageSize, QImage::Format_Grayscale8);
    this->pixmapItem = addPixmap(QPixmap::fromImage(this->image));

    this->startViewPoint = this->lastViewPoint = QVector3D(0, 0, -10);
    this->startViewUp = this->lastViewUp = QVector3D(0, 1, 0);

    viewportSize = QSizeF(5, 5);
    view = RView(lastViewPoint, lastViewUp);
    RDepthBuffer buffer = view.lookAt(world, imageSize);
    buffer.toImage(image, viewportSize);
    pixmapItem->setPixmap(QPixmap::fromImage(image));
}

void RealisticScene::setView(QVector3D viewPoint, double angle)
{
    view = RView(viewPoint, angle);
    RDepthBuffer buffer = view.lookAt(world, imageSize);
    buffer.toImage(image, viewportSize);
    pixmapItem->setPixmap(QPixmap::fromImage(image));
}

void RealisticScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << Q_FUNC_INFO << mouseEvent->scenePos();
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

/*static QMatrix4x4 eulerRotateZXZ(double alpha, double beta, double gamma) {
    using namespace std;
    double c1 = cos(alpha), s1 = sin(alpha);
    double c2 = cos(beta),  s2 = sin(beta);
    double c3 = cos(gamma), s3 = sin(gamma);
    QMatrix4x4 transform(
                c1*c3-c2*s1*s3, -c1*s3-c2*c3*s1, s1*s2,  0,
                c3*s1+c1*c2*s3, c1*c2*c3-s1*s3,  -c1*s2, 0,
                s2*s3,          c3*s2,           c2,     0,
                0,              0,               0,      1);
    return transform;
}

static QMatrix4x4 get3DAngle(QPointF start, QPointF end)
{
    double alpha, beta, gamma;
    QPointF diff = end - start;
    double length = std::hypot(diff.x(), diff.y());

    if (length > 0 && length <= 1) {
        double z0 = sqrt(1 - length * length);
        alpha = std::acos(diff.x() / length);
        beta = std::acos(diff.y() / length);
        gamma = std::acos(z0);
    }

    return eulerRotateZXZ(alpha, beta, gamma).inverted();
}*/

void RealisticScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    //qDebug() << Q_FUNC_INFO << mouseEvent->scenePos();
    QGraphicsScene::mouseMoveEvent(mouseEvent);

    QPointF position = (mouseEvent->scenePos() - center) / sphereRadius;
    QPointF diff = position - startPosition;
    double dist = std::hypot(diff.x(), diff.y());
    double angle = dist * 90;
    QMatrix4x4 rotate;

    if (std::abs(startPosition.x()) > std::abs(startPosition.y()))
        rotate.rotate(angle, 0, 1, 0);
    else
        rotate.rotate(angle, 1, 0, 0);

    /*QVector3D oldVec = QVector3D(startPosition.x(), startPosition.y(), startZ);
    QVector3D curVec = QVector3D(position.x(), position.y(), curZ);

    double norm = std::hypot(position.x(), position.y());
    double curZ = std::sqrt(1 - norm*norm);

    if (norm > 1) {
        position /= norm;
        curZ = 0.0;
    }

    QVector3D oldVec = QVector3D(startPosition.x(), startPosition.y(), startZ);
    QVector3D curVec = QVector3D(position.x(), position.y(), curZ);
    QVector3D diffVec = curVec - oldVec;

    QVector3D axis = QVector3D::crossProduct(diffVec, QVector3D(0, 0, 1));
    double theta = diffVec.length() * M_PI / 2;

    qDebug() << Q_FUNC_INFO << " AXIS:" << axis;
    QMatrix4x4 rotate = get3DAngle(startPosition, position);
    //rotate.rotate(theta * 180.0 / M_PI, axis.normalized());

    //rotate.rotate(diffVec.length() * 180.0 / M_PI, QVector3D(1, 0, 0));*/

    qDebug() << Q_FUNC_INFO << "Start:" << startViewPoint;
    qDebug() << Q_FUNC_INFO << " Last:" << lastViewPoint;
    lastViewPoint = rotate.map(startViewPoint);
    qDebug() << Q_FUNC_INFO << "  NOW:" << lastViewPoint;
    lastViewUp = rotate.map(startViewUp);

    view = RView(lastViewPoint, lastViewUp);
    RDepthBuffer buffer = view.lookAt(world, imageSize);
    buffer.toImage(image, viewportSize);
    pixmapItem->setPixmap(QPixmap::fromImage(image));
}

void RealisticScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << Q_FUNC_INFO << mouseEvent->scenePos();
    QGraphicsScene::mousePressEvent(mouseEvent);
    startPosition = (mouseEvent->scenePos() - center) / sphereRadius;

    double norm = std::hypot(startPosition.x(), startPosition.y());
    startZ = std::sqrt(1 - norm*norm);

    if (norm > 1) {
        startPosition /= norm;
        startZ = 0.0;
    }
}

void RealisticScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << Q_FUNC_INFO << mouseEvent->scenePos();
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
    this->startViewPoint = this->lastViewPoint;
    this->startViewUp = this->lastViewUp;
}
