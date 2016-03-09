#include "qigscene.h"

#include <QDebug>
#include <QGraphicsItemGroup>

#include "qiglayer.h"

QIGScene::QIGScene(QObject *parent) :
    QGraphicsScene(parent),
    mCurrentLayer(0)
{
}

QIGScene::~QIGScene()
{
}

bool QIGScene::addLayer(QIGLayer *aLayer)
{
    this->addItem(aLayer->mItem);
    mLayers.append(aLayer);
    return true;
}

void QIGScene::setCurrentLayer(const QString &layerName)
{

}

QIGLayer *QIGScene::currentLayer() const
{
    if (mCurrentLayer == 0 && !mLayers.isEmpty())
        mCurrentLayer = mLayers.last();

    return mCurrentLayer;
}

void QIGScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    if (currentLayer())
        currentLayer()->mousePressEvent(event);
}

void QIGScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
    if (currentLayer())
        currentLayer()->mouseMoveEvent(event);
}

void QIGScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if (currentLayer())
        currentLayer()->mouseReleaseEvent(event);
}

void QIGScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseDoubleClickEvent(event);
    if (currentLayer())
        currentLayer()->mouseDoubleClickEvent(event);
}
