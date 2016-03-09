#include "linecdpscaledraw.h"

LineCdpScaleDraw::LineCdpScaleDraw()
{
}

LineCdpScaleDraw::LineCdpScaleDraw(const SeisProfileData &seisProfileData,
                                   const QList<WellPositionInScale> &wellPositions) :
    mSeisProfileData(seisProfileData), mWellPositions(wellPositions), mMousePos(0.0)
{

}

void LineCdpScaleDraw::setSeisProfileData(const SeisProfileData &seisProfileData)
{
    mSeisProfileData = seisProfileData;
}

void LineCdpScaleDraw::setWellPosition(const QList<WellPositionInScale> &wellPositions)
{
    mWellPositions = wellPositions;
}

void LineCdpScaleDraw::setMousePosition(double mousePos)
{
    mMousePos = mousePos;
}

void LineCdpScaleDraw::draw(QPainter *painter, const QPalette &palette) const
{
    if (!mWellPositions.isEmpty())
    {
        painter->save();

        QPen pen(painter->pen());
        pen.setColor(Qt::red);
        painter->setPen(pen);
        painter->setRenderHint(QPainter::Antialiasing);
        foreach (const WellPositionInScale &aWell, mWellPositions)
        {
            double value = trace(aWell.line, aWell.cdp);
            QPointF pos = this->labelPosition(value);
            painter->drawEllipse(QPointF(pos.x(), pos.y()-28), 3.5, 3.5);
            painter->drawText(QPoint(pos.x()-20, pos.y()-35),  aWell.wellName);
        }

        painter->restore();
    }

    double mousePosToDevice = this->labelPosition(mMousePos).x();
    painter->drawLine(QPointF(mousePosToDevice, 0),  QPointF(mousePosToDevice, 60));

    QwtScaleDraw::draw(painter, palette);
}

QwtText LineCdpScaleDraw::label(double value) const
{
    int trace = static_cast<int>(value);
    if (trace>=0 && trace<mSeisProfileData.traceCount())
        return QString("%1\n%2").arg(mSeisProfileData.line(trace)).arg(mSeisProfileData.cdp(trace));

    return QwtText();
}
