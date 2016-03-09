#include "qiglayer.h"

#include <QDebug>
#include <QGraphicsItemGroup>

QIGLayer::QIGLayer(const QString &layerName, QObject *parent) :
    QObject(parent),
    mName(layerName),
    mItem(new QGraphicsItemGroup())
{
}

QIGLayer::~QIGLayer()
{
}

void QIGLayer::addChild(QGraphicsItem *item)
{
    mItem->addToGroup(item);
}

void QIGLayer::removeChild(QGraphicsItem *item)
{
    mItem->removeFromGroup(item);
}


