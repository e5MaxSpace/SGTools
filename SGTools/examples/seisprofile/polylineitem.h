#ifndef POLYLINEITEM_H
#define POLYLINEITEM_H

#include <QAbstractGraphicsShapeItem>

class PolylineItem : public QAbstractGraphicsShapeItem
{
public:
    explicit PolylineItem(QGraphicsItem *parent = 0);
    explicit PolylineItem(const QPolygonF &polygon, QGraphicsItem *parent = 0);

    QPolygonF polygon() const;
    void setPolygon(const QPolygonF &polygon);

    PolylineItem *clone() const;

protected:
    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    QPolygonF mPolygonF;

    // Cached bounding rectangle
    mutable QRectF mBoundingRect;
};

#endif // POLYLINEITEM_H
