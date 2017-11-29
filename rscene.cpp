#include "rview.h"
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

void RScene::addLight(const QVector3D origin, double bright, QSize bsize)
{
    RView view(origin, 0);
    QMatrix4x4 transform = view.getTransform();
    RDepthBuffer buffer = view.lookAt(*this, bsize, true);
    LightDescribe store = { buffer, transform, bright };
    lights.push_back(store);
}

double RScene::getBrightness(QVector3D position, int meshIndex) const
{
    double sum = baseBrightness;

    for (const LightDescribe &li: lights) {
        QVector3D mapped = li.transform.map(position);
        QPointF point = mapped.toPointF();
        QPoint pos = li.buffer.convertViewToPixel(point);
        double depth = li.buffer.getDepth(pos);
        int flag = li.buffer.getFlag(pos);

        if (flag == meshIndex) {
            double decayed = li.brightness / (depth * depth);
            sum += decayed;
        }
    }

    return sum;
}
