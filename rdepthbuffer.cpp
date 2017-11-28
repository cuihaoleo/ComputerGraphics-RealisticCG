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

    this->depthBuffer = QVector<double>(area, INFINITY);
    this->flagBuffer = QVector<int>(area, 0);
}

QPoint RDepthBuffer::convertViewToPixel(const QPointF &worldPoint) {
    QPointF shift = worldPoint - this->topLeft;
    double x = shift.x() * this->scaleX;
    double y = shift.y() * this->scaleY;
    return QPoint(std::round(x), std::round(y));
}

QPointF RDepthBuffer::convertPixelToView(const QPoint &pixelPosition) {
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

void RDepthBuffer::toDepthImage(QImage &im, const QSizeF &viewportSize)
{
    double scaleH = viewportSize.height() / im.height();
    double scaleW = viewportSize.width() / im.width();
    double pixelOffsetX = im.width() / 2.0;
    double pixelOffsetY = im.height() / 2.0;

    double maxZ = 1.0;
    for (int i=0; i<H(); i++)
        for (int j=0; j<W(); j++) {
            double z = getDepth(QPoint(j, i));
            if (std::isnormal(z) && z > maxZ)
                maxZ = z;
        }

    maxZ *= 1.1;
    for (int i=0; i<im.height(); i++) {
        uchar *scanline = im.scanLine(i);
        for (int j=0; j<im.width(); j++) {
            QPointF worldPoint((j - pixelOffsetY) * scaleW, (i - pixelOffsetX) * scaleH);
            QPoint pos = convertViewToPixel(worldPoint);

            double zval = INFINITY;

            if (VALID_IDX(pos.y(), pos.x()))
                zval = depthBuffer[IDX(pos.y(), pos.x())];

            if (std::isnormal(zval)) {
                scanline[j] = 255 * (1 - zval / maxZ);
            } else {
                scanline[j] = 0;
            }
        }
    }
}
