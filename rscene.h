#ifndef RSCENE_H
#define RSCENE_H

#include "rdepthbuffer.h"

#include <QVector>
#include <QVector3D>
#include <QSize>
#include <array>

class RScene
{
    friend class RView;
private:
    QVector<QVector3D> points;
    QVector<std::array<int, 3>> mesh;
    QVector<RDepthBuffer> lightBuffer;
public:
    RScene();
    bool addPoint(const QVector3D &point);
    void addTriangle(int ia, int ib, int ic);
    void addLight(const QVector3D origin, double bright, QSize bsize = QSize(1024, 1024));
};

#endif // RSCENE_H
