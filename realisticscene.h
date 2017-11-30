#ifndef REALISTICSCENE_H
#define REALISTICSCENE_H

#include "rscene.h"
#include "rview.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>

#include <QVector3D>
#include <QSizeF>

class RealisticScene : public QGraphicsScene
{
    Q_OBJECT

private:
    const RScene *world;
    RView view;

    QPointF startPosition;
    double startZ;
    QVector3D lastView;
    QImage image;
    QGraphicsPixmapItem *pixmapItem;
    QSize imageSize;
    QSizeF viewportSize;

public:
    explicit RealisticScene(const RScene *sence, const QSize &imageSize, QObject *parent = 0);
    void setView(QVector3D viewPoint, QVector3D viewUp);
};

#endif // REALISTICSCENE_H
