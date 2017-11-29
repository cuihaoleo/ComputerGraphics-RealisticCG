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

void RScene::addTriangle(int ia, int ib, int ic, const QVector3D &reflectBGR){
    std::array<int, 3> arr{ia, ib, ic};
    mesh.append(arr);
    reflect.append(reflectBGR);
}

void RScene::addLight(const QVector3D &origin, const QVector3D &light, QSize bsize)
{
    RView view(origin, 0);
    QMatrix4x4 transform = view.getTransform();
    RDepthBuffer buffer = view.lookAt(*this, bsize, true);
    LightDescribe store = { buffer, transform, light };
    lights.push_back(store);
}

QVector3D RScene::getLight(const QVector3D &position, int meshIndex) const
{
    QVector3D sum = baseLight;

    for (const LightDescribe &li: lights) {
        QVector3D mapped = li.transform.map(position);
        QPointF point = mapped.toPointF();
        QPoint pos = li.buffer.convertViewToPixel(point);
        double depth = li.buffer.getDepth(pos);
        int flag = li.buffer.getFlag(pos);

        if (flag == meshIndex) {
            QVector3D decayed = li.light / (depth * depth);
            sum += decayed;
        }
    }

    return sum;
}
