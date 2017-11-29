#ifndef RDEPTHBUFFER_H
#define RDEPTHBUFFER_H

#include <QPointF>
#include <QVector3D>
#include <QPoint>
#include <QSize>
#include <QVector>
#include <QImage>
#include <QSizeF>

#include <cassert>
#include <cmath>

class RDepthBuffer
{
private:
    QPointF topLeft, downRight;
    QSize bufferSize;

    double scaleX, scaleY;

    QVector<double> depthBuffer;
    QVector<QVector3D> lightBuffer;
    QVector<int> flagBuffer;

    int VALID_IDX(int i, int j) const {
        return i >= 0 && i < bufferSize.height() && j >= 0 && j < bufferSize.width();
    }

    int IDX(int i, int j) const {
        return i * bufferSize.height() + j;
    }

public:
    RDepthBuffer(const QSize &size, QPointF &topLeft, QPointF &downRight);
    QPoint convertViewToPixel(const QPointF &worldPoint) const;
    QPointF convertPixelToView(const QPoint &pixelPosition) const;

    bool update(const QPoint &pos, int flag, double depth, const QVector3D &light);
    bool update(const QPoint &pos, int flag, double depth, double light=1.0) {
        return update(pos, flag, depth, QVector3D(light, light, light));
    }

    void toDepthImage(QImage &im, const QSizeF &viewportSize);
    void toImage(QImage &im, const QSizeF &viewportSize);

    int W() const { return bufferSize.width(); }
    int H() const { return bufferSize.height(); }

    double getDepth(QPoint pos) const {
        if (VALID_IDX(pos.y(), pos.x()))
            return depthBuffer[IDX(pos.y(), pos.x())];
        else
            return INFINITY;
    }

    QVector3D getLight(QPoint pos) const {
        if (VALID_IDX(pos.y(), pos.x()))
            return lightBuffer[IDX(pos.y(), pos.x())];
        else
            return QVector3D();
    }

    int getFlag(QPoint pos) const {
        if (VALID_IDX(pos.y(), pos.x()))
            return flagBuffer[IDX(pos.y(), pos.x())];
        else
            return -1;
    }
};

#endif // RDEPTHBUFFER_H
