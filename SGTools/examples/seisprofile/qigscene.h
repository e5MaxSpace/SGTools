#ifndef QIGSCENE_H
#define QIGSCENE_H

#include <QGraphicsScene>

class QIGLayer;
class QIGScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum LayerInsertMode { limBelow, limAbove };
    Q_ENUMS(LayerInsertMode)

public:
    explicit QIGScene(QObject *parent = 0);
    ~QIGScene();

    bool addLayer(QIGLayer *aLayer);

    void setCurrentLayer(const QString &layerName);
    QIGLayer *currentLayer() const;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    QList<QIGLayer *> mLayers;
    mutable QIGLayer *mCurrentLayer;
};

#endif // QIGSCENE_H
