#ifndef RDEPTHBUFFER_H
#define RDEPTHBUFFER_H

#include <QPointF>
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
    QVector<double> lightBuffer;
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
    bool update(const QPoint &pos, int flag, double depth, double light=0.0);

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

    double getBrightness(QPoint pos) const {
        if (VALID_IDX(pos.y(), pos.x()))
            return lightBuffer[IDX(pos.y(), pos.x())];
        else
            return 0.0;
    }

    int getFlag(QPoint pos) const {
        if (VALID_IDX(pos.y(), pos.x()))
            return flagBuffer[IDX(pos.y(), pos.x())];
        else
            return -1;
    }
};

#endif // RDEPTHBUFFER_H
