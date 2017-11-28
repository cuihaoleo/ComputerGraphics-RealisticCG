#ifndef RVIEW_H
#define RVIEW_H

#include "rscene.h"
#include "rdepthbuffer.h"

#include <QImage>
#include <QVector3D>
#include <QMatrix4x4>

class RView
{
private:
    void constructor(const QVector3D &viewPoint, double angle);

protected:
    QVector3D normalVector;
    QMatrix4x4 viewTransform;
    QMatrix4x4 viewTransformR;

public:
    RView() {}

    RView(const QVector3D &viewPoint, double angle) {
        constructor(viewPoint, angle);
    }

    RView(const QVector3D &viewPoint, const QVector3D &viewUp);

    RDepthBuffer lookAt(const RScene &scene, const QSize &bufferSize);
};


#endif // RVIEW_H
