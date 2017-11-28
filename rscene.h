#ifndef RSCENE_H
#define RSCENE_H

//#include "rview.h"

#include <QVector>
#include <QVector3D>
#include <array>

class RScene
{
    friend class RView;
private:
    QVector<QVector3D> points;
    QVector<std::array<int, 3>> mesh;
public:
    RScene();
    bool addPoint(const QVector3D &point);
    void addTriangle(int ia, int ib, int ic);
};

#endif // RSCENE_H
