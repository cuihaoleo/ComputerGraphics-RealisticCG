#include "rscene.h"
#include "rview.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <climits>

#include <list>
#include <algorithm>
#include <memory>

#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QSize>

static QVector4D solvePlane(
        const QVector3D &p1, const QVector3D &p2, const QVector3D &p3 ) {
    QVector3D v1 = p2 - p1;
    QVector3D v2 = p3 - p2;
    QVector3D n = QVector3D::crossProduct(v1, v2);
    double d = -QVector3D::dotProduct(n, p2);
    return QVector4D(n, d);
}

static QMatrix4x4 rotateMatrixBetween(const QVector3D &a, const QVector3D &b) {
    QVector3D cross = QVector3D::crossProduct(a, b);
    double dot = QVector3D::dotProduct(a, b);
    double s = a.length() * b.length() + dot;
    QMatrix4x4 ret;

    if (std::abs(s) < std::numeric_limits<float>::epsilon()) {
        if (dot < 0)
            ret(0, 0) = ret(1, 1) = ret(2, 2) = -1.0;
    } else {
        QQuaternion q(s, cross);
        ret.rotate(q.normalized());
    }

    return ret;
}

void RView::constructor(const QVector3D &viewPoint, double angle)
{
    QVector3D view = -viewPoint.normalized();
    static const QVector3D axisZ(0, 0, 1);
    double degree = angle * 180.0 / M_PI;

    QMatrix4x4 rotate1 = rotateMatrixBetween(view, axisZ); // rotate1: view in world -> z'

    QMatrix4x4 rotate2;
    rotate2.rotate(-degree, axisZ);  // rotate2: x, y rotate

    QMatrix4x4 translate;
    QVector3D newZ = rotate1.map(-viewPoint);
    translate.translate(0, 0, newZ.z());  // move origin

    QMatrix4x4 combine = translate * rotate2 * rotate1;
    combine.optimize();  // combile: world -> view

    this->normalVector = view.normalized();
    this->viewTransform = combine;
    this->viewTransformR = combine.inverted();
}

RView::RView(const QVector3D &viewPoint, const QVector3D &viewUp)
{
    QVector3D view = -viewPoint.normalized();
    static const QVector3D axisZ(0, 0, 1);
    static const QVector3D axisY(0, 1, 0);

    QMatrix4x4 rotate1 = rotateMatrixBetween(view, axisZ); // rotate1: view in world -> z'

    QVector3D viewUp1 = rotate1.map(viewUp);
    QMatrix4x4 rotate2 = rotateMatrixBetween(viewUp1, axisY);
    rotate2(2, 2) = 1.0;  // rotate2: x, y rotate

    QMatrix4x4 translate;
    QVector3D newZ = rotate1.map(-viewPoint);
    translate.translate(0, 0, newZ.z());  // move origin

    QMatrix4x4 combine = translate * rotate2 * rotate1;
    combine.optimize();  // combile: world -> view

    this->normalVector = view.normalized();
    this->viewTransform = combine;
    this->viewTransformR = combine.inverted();
}

RDepthBuffer RView::lookAt(const RScene &scene, const QSize &bufferSize, bool viewOnly)
{
    std::vector<QVector3D> viewPoints;
    double maxX, maxY;
    double minX, minY;

    maxX = maxY = -INFINITY;
    minX = minY = INFINITY;

    for (const QVector3D &p: scene.points) {
        QVector3D moved = this->viewTransform.map(p);
        if (moved.x() > maxX) maxX = moved.x();
        if (moved.x() < minX) minX = moved.x();
        if (moved.y() > maxY) maxY = moved.y();
        if (moved.y() < minY) minY = moved.y();
        viewPoints.push_back(moved);
    }

    QPointF topLeft(minX - 1, minY - 1);
    QPointF downRight(maxX + 1, maxY + 1);

    RDepthBuffer buffer(bufferSize, topLeft, downRight);

    std::vector<QPoint> drawPoints;
    std::vector<double> drawZ;
    for (const QVector3D &p: viewPoints) {
        QPoint dp = buffer.convertViewToPixel(p.toPointF());
        drawPoints.push_back(dp);
        drawZ.push_back(p.z());
    }

    struct PTItem {
        int idx;
        int ymax;
    };
    std::vector<std::list<PTItem>> PT(bufferSize.height());

    for (int i=0; i<int(scene.mesh.size()); i++) {
        const QPoint &pa = drawPoints[scene.mesh[i][0]];
        const QPoint &pb = drawPoints[scene.mesh[i][1]];
        const QPoint &pc = drawPoints[scene.mesh[i][2]];

        int ymin = std::min({pa.y(), pb.y(), pc.y()});
        int ymax = std::max({pa.y(), pb.y(), pc.y()});

        PT[ymin].push_back(PTItem{ i, ymax });
    }

    struct APTItem {
        int index;
        QVector4D factor;
        double kab, kac, kbc;
        int ymax, yturn;
        double xstart, xend;
        double zmin, zmax;
        int xturn;
    };
    std::list<APTItem> APT;

    for (int y=0; y<bufferSize.height(); y++) {

        // Add new items to APT
        for (PTItem &store: PT[y]) {
            int idx = store.idx, ymax = store.ymax;
            const std::array<int, 3> &mesh = scene.mesh[idx];
            double z0 = drawZ[mesh[0]], z1 = drawZ[mesh[1]], z2 = drawZ[mesh[2]];

            QVector3D pa(drawPoints[mesh[0]]);
            pa.setZ(z0);
            QVector3D pb(drawPoints[mesh[1]]);
            pb.setZ(z1);
            QVector3D pc(drawPoints[mesh[2]]);
            pc.setZ(z2);

            // sort
            if (pa.y() > pb.y()) std::swap(pa, pb);
            if (pa.y() > pc.y()) std::swap(pa, pc);
            if (pb.y() > pc.y()) std::swap(pb, pc);

            QVector4D solution = solvePlane(pa, pb, pc);

            APTItem item;
            item.index = idx;
            item.factor = solution;
            item.ymax = ymax;
            item.yturn = std::round(pb.y());
            item.kab = (pa.x() - pb.x()) / (pa.y() - pb.y());
            item.kac = (pa.x() - pc.x()) / (pa.y() - pc.y());
            item.kbc = (pb.x() - pc.x()) / (pb.y() - pc.y());
            item.xstart = item.xend = pa.x();
            item.xturn = std::round(pb.x());
            item.zmin = std::min({z0, z1, z2});
            item.zmax = std::max({z0, z1, z2});
            APT.push_back(item);
        }

        // scanline
        std::list<APTItem>::iterator iter = APT.begin();
        while (iter != APT.end()) {
            if (y > iter->ymax) {
                iter = APT.erase(iter);
                continue;
            }

            double delta_yend = y < iter->yturn ? iter->kab : iter->kbc;
            int index = iter->index;
            int xstart = std::round(qMin(iter->xstart, iter->xend));
            int xend = std::round(qMax(iter->xstart, iter->xend));
            double zmin = iter->zmin, zmax = iter->zmax;
            double A = iter->factor[0];
            double B = iter->factor[1];
            double C = iter->factor[2];
            double D = iter->factor[3];

            double z = (-A*xstart - B*y - D) / C; // z at (xstart, y)
            for (int x=xstart; x<=xend; x++, z -= A / C) {
                double zmod = std::max(zmin, std::min(z, zmax));

                if (viewOnly)
                    buffer.update(QPoint(x, y), index, zmod, scene.reflect[index]);
                else {
                    QPointF view = buffer.convertPixelToView(QPoint(x, y));
                    QVector3D world = viewTransformR.map(QVector3D(view.x(), view.y(), zmod));
                    QVector3D light = scene.getLight(world, index);
                    QVector3D decayed = light / (1 + zmod * zmod);
                    buffer.update(QPoint(x, y), index, zmod, decayed);
                }
            }

            iter->xstart += iter->kac;
            iter->xend += delta_yend;
            if (y == iter->yturn) iter->xend = iter->xturn;
            iter++;
        }
    }

    return buffer;
}
