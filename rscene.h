#ifndef RSCENE_H
#define RSCENE_H

#include "rdepthbuffer.h"

#include <array>
#include <memory>
#include <vector>

#include <QMatrix4x4>
#include <QVector3D>
#include <QSize>

class RScene
{
    friend class RView;
private:
    struct LightDescribe {
        RDepthBuffer buffer;
        QMatrix4x4 transform;
        QVector3D light;
    };

    std::vector<QVector3D> points;
    std::vector<std::array<int, 3>> mesh;
    std::vector<QVector3D> reflect;
    std::vector<LightDescribe> lights;
    QVector3D baseLight = QVector3D(0, 0, 0);

public:
    RScene() = default;

    void setBaseLight(const QVector3D light) {
        baseLight = light;
    }

    void setBaseLight(double light) {
        setBaseLight(QVector3D(light, light, light));
    }

    int addPoint(const QVector3D &point);

    void addTriangle(int ia, int ib, int ic, const QVector3D &reflectBGR);
    void addTriangle(int ia, int ib, int ic, double r = 1.0) {
        addTriangle(ia, ib, ic, QVector3D(r, r, r));
    }

    void addPolygon(std::initializer_list<int> args, const QVector3D &reflectBGR);
    void addPolygon(std::initializer_list<int> args, double r = 1.0) {
        addPolygon(args, QVector3D(r, r, r));
    }

    void addLight(const QVector3D &origin, const QVector3D &lightBGR, QSize bsize = QSize(1024, 1024));
    void addLight(const QVector3D &origin, double light, QSize bsize = QSize(1024, 1024)) {
        addLight(origin, QVector3D(light, light, light), bsize);
    }

    void clearLights() {
        lights.clear();
    }

    QVector3D getLight(const QVector3D &position, int meshIndex) const;
};

#endif // RSCENE_H
