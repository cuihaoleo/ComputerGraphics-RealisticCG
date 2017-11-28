#ifndef RDEPTHBUFFER_H
#define RDEPTHBUFFER_H

#include <QPointF>
#include <QSize>
#include <QVector>
#include <QImage>
#include <QSizeF>

#include <cassert>

class RDepthBuffer
{
private:
    QPointF topLeft, downRight;
    QSize bufferSize;

    double scaleX, scaleY;

    QVector<double> depthBuffer;
    QVector<int> flagBuffer;

    int VALID_IDX(int i, int j) {
        return i >= 0 && i < bufferSize.height() && j >= 0 && j < bufferSize.width();
    }

    int IDX(int i, int j) {
        return i * bufferSize.height() + j;
    }

public:
    RDepthBuffer(const QSize &size, QPointF &topLeft, QPointF &downRight);
    QPoint convertViewToPixel(const QPointF &worldPoint);
    QPointF convertPixelToView(const QPoint &pixelPosition);
    bool update(const QPoint &pos, double depth, int flag);

    void toDepthImage(QImage &im, const QSizeF &viewportSize);

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
