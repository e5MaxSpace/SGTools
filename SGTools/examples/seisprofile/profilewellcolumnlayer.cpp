#include "profilewellcolumnlayer.h"

#include <QPen>
#include <QGraphicsLineItem>

ProfileWellColumnLayer::ProfileWellColumnLayer(QObject *parent) :
    QIGLayer(QLatin1String("ProfileWellColumnLayer"), parent)
{
}

void ProfileWellColumnLayer::addWell(int pos, float startTime, float endTime)
{
    QGraphicsLineItem *wellItem = new QGraphicsLineItem(QLineF(QPointF(pos, startTime), QPointF(pos, endTime)));
    QPen pen(wellItem->pen());
    pen.setColor(Qt::blue);
    wellItem->setPen(pen);
    this->addChild(wellItem);
}
