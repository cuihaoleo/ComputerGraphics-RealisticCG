#ifndef RSCENE_H
#define RSCENE_H

#include "rdepthbuffer.h"

#include <QMatrix4x4>
#include <QPair>
#include <QVector>
#include <QVector3D>
#include <QSize>

#include <array>
#include <memory>
#include <vector>

#include <cassert>

class RScene
{
    friend class RView;
private:
    struct LightDescribe {
        RDepthBuffer buffer;
        QMatrix4x4 transform;
        double brightness;
    };

    QVector<QVector3D> points;
    QVector<std::array<int, 3>> mesh;
    std::vector<LightDescribe> lights;
    double baseBrightness = 0.0;

public:
    RScene();

    void setBaseBrightness(double bright) {
        assert(bright >= 0.0);
        baseBrightness = bright;
    }

    bool addPoint(const QVector3D &point);
    void addTriangle(int ia, int ib, int ic);
    void addLight(const QVector3D origin, double bright, QSize bsize = QSize(1024, 1024));

    double getBrightness(QVector3D position, int meshIndex) const;
};

#endif // RSCENE_H
