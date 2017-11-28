#ifndef RLIGHTVIEW_H
#define RLIGHTVIEW_H

#include "rscene.h"
#include "rview.h"

class RLightView: private RView
{
private:
    double brightness;
    QSize bufferSize;

public:
    RLightView(QVector3D origin, double bright, QSize bsize):
        RView(origin, 0), brightness(bright), bufferSize(bsize) {
    }

    void shine(RScene &world) {
        lookAt(world, bufferSize);
    }

    double getReflection(QVector3D &world, int flag) {
        QVector3D mapped = viewTransform.map(world);
        QPoint pixel = buffer->convertWorldToPixel(mapped.toPointF());
        double depth = buffer->getDepth(pixel);
        int myflag = buffer->getFlag(pixel);
        return myflag == flag ? depth : 0.0;
    }
};

#endif // RLIGHTVIEW_H
