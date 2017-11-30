#include "rdepthbuffer.h"

#include <QDebug>
#include <QRgb>

#define _USE_MATH_DEFINES
#include <cmath>

RDepthBuffer::RDepthBuffer(const QSize &size, QPointF &topLeft, QPointF &downRight) {
    this->topLeft = topLeft;
    this->downRight = downRight;
    this->bufferSize = size;
    int area = size.height() * size.width();
    QPointF diff = downRight - topLeft;

    this->scaleX = size.width() / diff.x();
    this->scaleY = size.height() / diff.y();

    this->depthBuffer = std::vector<double>(area, INFINITY);
    this->lightBuffer = std::vector<QVector3D>(area, QVector3D());
    this->flagBuffer = std::vector<int>(area, 0);
}

QPoint RDepthBuffer::convertViewToPixel(const QPointF &worldPoint) const {
    QPointF shift = worldPoint - this->topLeft;
    double x = shift.x() * this->scaleX;
    double y = shift.y() * this->scaleY;
    return QPoint(std::round(x), std::round(y));
}

QPointF RDepthBuffer::convertPixelToView(const QPoint &pixelPosition) const {
    double x = pixelPosition.x() / this->scaleX;
    double y = pixelPosition.y() / this->scaleY;
    return QPointF(x, y) + this->topLeft;
}

bool RDepthBuffer::update(const QPoint &pos, int flag, double depth, const QVector3D &light) {
    int index = IDX(pos.y(), pos.x());
    double oldDepth = this->depthBuffer[index];

    if (depth < oldDepth && depth > 0) {
        this->flagBuffer[index] = flag;
        this->depthBuffer[index] = depth;
        this->lightBuffer[index] = light;
        return true;
    }

    return false;
}

void RDepthBuffer::toImage(QImage &im, const QSizeF &viewportSize) const
{
    double scaleH = viewportSize.height() / im.height();
    double scaleW = viewportSize.width() / im.width();
    double pixelOffsetX = im.width() / 2.0;
    double pixelOffsetY = im.height() / 2.0;

    double maxLight = 0.0;
    for (int i=0; i<H(); i++)
        for (int j=0; j<W(); j++) {
            QVector3D light = getLight(QPoint(j, i));
            for (int k=0; k<3; k++)
                if (std::isnormal(light[k]) && light[k] > maxLight)
                    maxLight = light[k];
        }

    for (int i=0; i<im.height(); i++) {
        QRgb *scanline = (QRgb*)im.scanLine(i);
        for (int j=0; j<im.width(); j++) {
            QPointF worldPoint((j - pixelOffsetX) * scaleH, (i - pixelOffsetY) * scaleW);
            QPoint pos = convertViewToPixel(worldPoint);

            QVector3D light = getLight(pos);
            int bval = 255 * light[0] / maxLight;
            int gval = 255 * light[1] / maxLight;
            int rval = 255 * light[2] / maxLight;

            scanline[j] = qRgb(rval, gval, bval);
        }
    }
}
