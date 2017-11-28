#include "rdepthbuffer.h"

#include <cmath>

RDepthBuffer::RDepthBuffer(const QSize &size, QPointF &topLeft, QPointF &downRight) {
    this->topLeft = topLeft;
    this->downRight = downRight;
    this->bufferSize = size;
    int area = size.height() * size.width();
    QPointF diff = downRight - topLeft;

    this->scaleX = size.width() / diff.x();
    this->scaleY = size.height() / diff.y();

    this->depthBuffer = new double[area];
    std::fill_n(this->depthBuffer, area, INFINITY);
    this->flagBuffer = new int[area];
    std::fill_n(this->flagBuffer, area, 0);
}

QPoint RDepthBuffer::convertWorldToPixel(const QPointF &worldPoint) {
    QPointF shift = worldPoint - this->topLeft;
    double x = shift.x() * this->scaleX;
    double y = shift.y() * this->scaleY;
    return QPoint(std::round(x), std::round(y));
}

QPointF RDepthBuffer::convertPixelToWorld(const QPoint &pixelPosition) {
    double x = pixelPosition.x() / this->scaleX;
    double y = pixelPosition.y() / this->scaleY;
    return QPointF(x, y) + this->topLeft;
}

bool RDepthBuffer::update(const QPoint &pos, double depth, int flag) {
    int index = IDX(pos.y(), pos.x());
    double oldDepth = this->depthBuffer[index];

    if (depth < oldDepth && depth > 0) {
        this->depthBuffer[index] = depth;
        this->flagBuffer[index] = flag;
        return true;
    }

    return false;
}
