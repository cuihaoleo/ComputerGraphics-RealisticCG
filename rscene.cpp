#include "rview.h"
#include "rscene.h"

#include <QPolygon>

#include <cassert>

RScene::RScene()
{

}

int RScene::addPoint(const QVector3D &point) {
    int index = points.indexOf(point);
    if (index < 0) {
        index = points.size();
        points.append(point);
    }

    return index;
}

void RScene::addTriangle(int ia, int ib, int ic, const QVector3D &reflectBGR){
    std::array<int, 3> arr{ia, ib, ic};
    mesh.append(arr);
    reflect.append(reflectBGR);
}

void RScene::addPolygon(std::initializer_list<int> args, const QVector3D &reflectBGR)
{
    int n_args = args.size();
    assert(n_args >= 3);

    auto p = args.begin();
    int ia = *(p++);
    int ib = *(p++);

    while (p != args.end()) {
        int ic = *(p++);
        addTriangle(ia, ib, ic, reflectBGR);
        ib = ic;
    }
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
    QVector3D ref = reflect[meshIndex];
    QVector3D sum = baseLight * ref;

    for (const LightDescribe &li: lights) {
        QVector3D mapped = li.transform.map(position);
        QPointF point = mapped.toPointF();
        QPoint pos = li.buffer.convertViewToPixel(point);
        double depth = li.buffer.getDepth(pos);
        int flag = li.buffer.getFlag(pos);
        ref = li.buffer.getLight(pos);

        if (flag == meshIndex) {
            QVector3D decayed = li.light * ref / (1 + depth * depth);
            sum += decayed;
        }
    }

    return sum;
}
