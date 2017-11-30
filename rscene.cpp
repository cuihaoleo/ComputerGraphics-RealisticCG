#include "rview.h"
#include "rscene.h"

#include <cassert>

#include <QPolygon>

int RScene::addPoint(const QVector3D &point) {
    auto it = std::find(points.begin(), points.end(), point);

    if (it == points.end()) {
        points.push_back(point);
        return int(points.size() - 1);
    }

    return it - points.begin();
}

void RScene::addTriangle(int ia, int ib, int ic, const QVector3D &reflectBGR){
    std::array<int, 3> arr{ia, ib, ic};
    mesh.push_back(arr);
    reflect.push_back(reflectBGR);
}

void RScene::addPolygon(std::initializer_list<int> args, const QVector3D &reflectBGR)
{
    assert(args.size() >= 3);

    auto p = args.begin();
    int ia = *(p++);
    int ib = *(p++);

    while (p != args.end()) {
        int ic = *(p++);
        addTriangle(ia, ib, ic, reflectBGR);
        ib = ic;
    }
}

void RScene::addLight(const QVector3D &origin, const QVector3D &lightBGR, QSize bsize)
{
    RView view(origin, 0);
    QMatrix4x4 transform = view.getTransform();
    RDepthBuffer buffer = view.lookAt(*this, bsize, true);
    LightDescribe store = { buffer, transform, lightBGR };
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
