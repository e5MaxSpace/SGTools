#ifndef PROFILESEISHORIZONLAYER_H
#define PROFILESEISHORIZONLAYER_H

#include "qiglayer.h"

class PolylineItem;

class ProfileSeisHorizonLayer : public QIGLayer
{
    Q_OBJECT

public:
    explicit ProfileSeisHorizonLayer(QObject *parent = 0);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    PolylineItem *mEditedItem;
};

#endif // PROFILESEISHORIZONLAYER_H
