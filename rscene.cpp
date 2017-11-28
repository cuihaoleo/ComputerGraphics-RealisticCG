#include "rscene.h"

#include <QPolygon>

RScene::RScene()
{

}

bool RScene::addPoint(const QVector3D &point) {
    if (points.contains(point))
        return false;
    else {
        points.append(point);
        return true;
    }
}

void RScene::addTriangle(int ia, int ib, int ic){
    std::array<int, 3> arr{ia, ib, ic};
    mesh.append(arr);
}
