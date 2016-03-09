#ifndef QIGLAYER_H
#define QIGLAYER_H

#include <QObject>

class QIGScene;
class QGraphicsItemGroup;
class QGraphicsItem;
class QGraphicsSceneMouseEvent;

class QIGLayer : public QObject
{
    Q_OBJECT

public:
    explicit QIGLayer(const QString &layerName, QObject *parent = 0);
    ~QIGLayer();

    QString name() const { return mName; }

    void addChild(QGraphicsItem *item);
    void removeChild(QGraphicsItem *item);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent */*event*/)
    {

    }

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent */*event*/)
    {

    }

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent */*event*/)
    {

    }

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent */*event*/)
    {

    }

private:
    QString mName;
    QGraphicsItemGroup *mItem;

private:
    Q_DISABLE_COPY(QIGLayer)

    friend class QIGScene;
};

#endif // QIGLAYER_H
