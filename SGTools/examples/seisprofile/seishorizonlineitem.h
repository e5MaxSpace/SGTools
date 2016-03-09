#ifndef SEISHORIZONLINEITEM_H
#define SEISHORIZONLINEITEM_H

#include <QGraphicsItemGroup>

class PolylineItem;
class SeisHorizonLineItem : public QGraphicsItemGroup
{
public:
    explicit SeisHorizonLineItem(const QList<QPointF> &line, QGraphicsItem *parent = 0);

    QPen pen() const;
    void setPen(const QPen &pen);

private:
    QList<PolylineItem*> mSegmentItems;
};

#endif // SEISHORIZONLINEITEM_H
