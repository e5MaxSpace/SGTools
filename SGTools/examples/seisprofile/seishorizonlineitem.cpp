#include "seishorizonlineitem.h"

#include <QPen>

//#include "qigglobal.h"
#include "polylineitem.h"

static QList<QPolygonF> splitToPolygons(const QList<QPointF> &line)
{
    QList<QPolygonF> results;
    QPolygonF polygon;
    foreach (const QPointF &pt, line)
    {
//        if (!QIG::isNullValue(pt.y())) // y为时间轴或者深度轴
//        {
//            polygon.append(pt);
//        }
//        else if (!polygon.isEmpty())
//        {
//            results.append(polygon);
//            polygon.clear();
//        }
    }

    if (!polygon.isEmpty())
    {
        results.append(polygon);
    }

    return results;
}

SeisHorizonLineItem::SeisHorizonLineItem(const QList<QPointF> &line, QGraphicsItem *parent) :
    QGraphicsItemGroup(parent)
{
    QList<QPolygonF> polygons(splitToPolygons(line));
    foreach (const QPolygonF &polygon, polygons)
    {
        PolylineItem *segmentItem = new PolylineItem(polygon, this);

        QPen oldPen(segmentItem->pen());
        oldPen.setColor(Qt::red);
        segmentItem->setPen(oldPen);

        this->mSegmentItems.append(segmentItem);
        this->addToGroup(segmentItem);
    }
}

QPen SeisHorizonLineItem::pen() const
{
    if (mSegmentItems.isEmpty())
        return QPen();

    return mSegmentItems.first()->pen();
}

void SeisHorizonLineItem::setPen(const QPen &pen)
{
    if (pen != this->pen())
    {
        foreach (PolylineItem *segmentItem, mSegmentItems)
            segmentItem->setPen(pen);
    }
}
