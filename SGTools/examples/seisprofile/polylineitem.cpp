#include "polylineitem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

PolylineItem::PolylineItem(QGraphicsItem *parent) :
    QAbstractGraphicsShapeItem(parent)
{
}

PolylineItem::PolylineItem(const QPolygonF &polygon, QGraphicsItem *parent) :
    QAbstractGraphicsShapeItem(parent), mPolygonF(polygon)
{

}

QPolygonF PolylineItem::polygon() const
{
    return mPolygonF;
}

void PolylineItem::setPolygon(const QPolygonF &polygon)
{
    if (mPolygonF == polygon)
        return;
    prepareGeometryChange();
    mPolygonF = polygon;
    mBoundingRect = QRectF();
    update();
}

PolylineItem *PolylineItem::clone() const
{
    PolylineItem *item = new PolylineItem(this->polygon());
    item->setPen(this->pen());
    item->setBrush(this->brush());
    return item;
}

QRectF PolylineItem::boundingRect() const
{
    if (mBoundingRect.isNull())
    {
        //qreal pw = pen().widthF();
        //if (pw == 0.0)
        mBoundingRect = QPolygonF(mPolygonF).boundingRect();
        //else
        //mBoundingRect = shape().controlPointRect();
    }

    return mBoundingRect;
}

QPainterPath PolylineItem::shape() const
{
    QPainterPath path;
    path.addPolygon(QPolygonF(mPolygonF));
    return path;
}

void PolylineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(this->pen());
    painter->setBrush(this->brush());
    painter->drawPolyline(mPolygonF.data(), mPolygonF.size());
    painter->restore();
}
