#ifndef REALISTICSCENE_H
#define REALISTICSCENE_H

#include "rscene.h"
#include "rview.h"

#include <QVector3D>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>

class RealisticScene : public QGraphicsScene
{
    Q_OBJECT

private:
    RScene world;
    RView view;

    QVector3D startViewPoint;
    QVector3D lastViewPoint;

    QVector3D startViewUp;
    QVector3D lastViewUp;

    QPointF startPosition;
    double startZ;
    QVector3D lastView;
    QImage image;
    QGraphicsPixmapItem *pixmapItem;
    QSize imageSize;
    QPointF center;
    double sphereRadius;

public:
    explicit RealisticScene(const RScene &sence, const QSize &imageSize, QObject *parent = 0);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
};

#endif // REALISTICSCENE_H
