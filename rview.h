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
    QVector3D normalVector;
    QMatrix4x4 viewTransform;
    QMatrix4x4 viewTransformR;
    RDepthBuffer *buffer = nullptr;

    void constructor(const QVector3D &viewPoint, double angle);

public:
    RView() {}

    RView(const QVector3D &viewPoint, double angle) {
        constructor(viewPoint, angle);
    };

    RView(const QVector3D &viewPoint, const QVector3D &viewUp);

    ~RView() {
        if (buffer != nullptr) delete buffer;
    }

    void lookAt(RScene scene, const QSize &bufferSize);
    void toDepthImage(QImage &im);
};


#endif // RVIEW_H
