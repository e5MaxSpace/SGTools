#include "profileseishorizonlayer.h"

#include <QPen>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

#include "polylineitem.h"

ProfileSeisHorizonLayer::ProfileSeisHorizonLayer(QObject *parent) :
    QIGLayer(QLatin1String("ProfileSeisHorizonLayer"), parent)
{
    mEditedItem = new PolylineItem();
    QPen pen(mEditedItem->pen());
    pen.setWidth(1);
    pen.setColor(Qt::green);
    mEditedItem->setPen(pen);
    this->addChild(mEditedItem);
}

void ProfileSeisHorizonLayer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mEditedItem->setVisible(true);
        QPointF curPoint = event->scenePos();
        QPolygonF polygonF(mEditedItem->polygon());
        polygonF.append(curPoint);
        mEditedItem->setPolygon(polygonF);
    }
}

void ProfileSeisHorizonLayer::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        this->addChild(mEditedItem->clone());
        mEditedItem->setPolygon(QPolygonF());
        mEditedItem->setVisible(false);
    }
}
