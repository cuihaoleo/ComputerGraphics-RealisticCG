#ifndef RDEPTHBUFFER_H
#define RDEPTHBUFFER_H

#include <QPointF>
#include <QSize>

#include <cassert>

class RDepthBuffer
{
private:
    QPointF topLeft, downRight;
    QSize bufferSize;

    double scaleX, scaleY;

    double *depthBuffer;
    int *flagBuffer;

    int VALID_IDX(int i, int j) {
        return i >= 0 && i < bufferSize.height() && j >= 0 && j < bufferSize.width();
    }

    int IDX(int i, int j) {
        assert (i >= 0 && i < bufferSize.height());
        assert (j >= 0 && j < bufferSize.width());
        return i * bufferSize.height() + j;
    }

public:
    RDepthBuffer(const QSize &size, QPointF &topLeft, QPointF &downRight);
    QPoint convertWorldToPixel(const QPointF &worldPoint);
    QPointF convertPixelToWorld(const QPoint &pixelPosition);
    bool update(const QPoint &pos, double depth, int flag);

    int W() { return bufferSize.width(); }
    int H() { return bufferSize.height(); }

    double getDepth(QPoint pos) {
        if (VALID_IDX(pos.y(), pos.x()))
            return depthBuffer[IDX(pos.y(), pos.x())];
        else
            return INFINITY;
    }

    int getFlag(QPoint pos) {
        if (VALID_IDX(pos.y(), pos.x()))
            return depthBuffer[IDX(pos.y(), pos.x())];
        else
            return -1;
    }
};

#endif // RDEPTHBUFFER_H
