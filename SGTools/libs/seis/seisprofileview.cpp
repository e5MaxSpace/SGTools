#include "seisprofileview.h"

#include <qmath.h>
#include <QDebug>
#include <QMouseEvent>
#include <QGraphicsScene>

#include <qwt_scale_widget.h>
#include <qwt_scale_div.h>
#include <qwt_scale_engine.h>

#include "hintmousescalewidget.h"
#include "linecdpscaledraw.h"
#include "seisprofiledata.h"
#include "seisprofileitem.h"

VScaleFilter::VScaleFilter(QObject * parent) : QObject(parent)
{

}

bool VScaleFilter::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::Wheel)
    {
        QWheelEvent *e = static_cast<QWheelEvent *>(event);
        int numDegrees = e->delta() / 8.0;
        int numSteps = numDegrees / 15.0;
        emit scale(numSteps*0.1+1.0);
    }

    return  QObject::eventFilter(obj,event);
}

SeisProfileView::SeisProfileView(QWidget *parent) :
    QGraphicsView(parent),
    viewTopMarginHeight(60), viewBotMarginHeight(0), viewLeftMarginWidth(60), viewRightMarginWidth(60)
{
    mScene = new QGraphicsScene(this);
    this->setScene(mScene);

    mSeisItem = new SeisProfileItem();
    mScene->addItem(mSeisItem);

    VScaleFilter *hsf = new VScaleFilter(this);
    topScale = new QwtScaleWidget(QwtScaleDraw::TopScale,this);
    topScale->installEventFilter(hsf);
    topScale->setMargin(0);
    topScale->setBorderDist(viewLeftMarginWidth, viewRightMarginWidth);
    topScale->setTitle("line/cdp");

    topScaleDraw = new LineCdpScaleDraw();
    topScale->setScaleDraw(topScaleDraw);

    VScaleFilter *vsf = new VScaleFilter(this);
    leftScale = new HintMouseScaleWidget(QwtScaleDraw::LeftScale, this);
    leftScale->installEventFilter(vsf);
    leftScale->setMargin(0);
    leftScale->setBorderDist(viewTopMarginHeight, viewBotMarginHeight);
    leftScale->setLabelAlignment(Qt::AlignLeft);
    leftScale->setTitle("Time (ms)");

    rightScale = new HintMouseScaleWidget(QwtScaleDraw::RightScale, this);
    rightScale->installEventFilter(vsf);
    rightScale->setMargin(0);
    rightScale->setBorderDist(viewTopMarginHeight, viewBotMarginHeight);
    rightScale->setLabelAlignment(Qt::AlignRight);
    rightScale->setTitle("Time (ms)");

    connect(vsf, SIGNAL(scale(float)), this, SLOT(zoomVertical(float)));
    connect(hsf, SIGNAL(scale(float)), this, SLOT(zoomHorizontal(float)));

    setViewportMargins(viewLeftMarginWidth, viewTopMarginHeight, viewLeftMarginWidth, viewBotMarginHeight);
    setFrameStyle(QFrame::NoFrame);
    setAcceptDrops(false);
    //setDragMode(QGraphicsView::ScrollHandDrag);
    setMouseTracking(true);
}

SeisProfileView::~SeisProfileView()
{
}

void SeisProfileView::setSeisProfileData(const SeisProfileData &seisProfileData)
{
    mScene->setSceneRect(0, 0, seisProfileData.traceCount(), 2000);
    mSeisItem->setProfileData(seisProfileData.profileData());
    topScaleDraw->setSeisProfileData(seisProfileData);
}

void SeisProfileView::setWellPosition(const QList<WellPositionInScale> &wellPositions)
{
    topScaleDraw->setWellPosition(wellPositions);
}

void SeisProfileView::setColorMap(const QVector<QRgb> &vecColor)
{
    mSeisItem->setColorMap(vecColor);
}

void SeisProfileView::varDensity()
{
    mSeisItem->setDisplayMode(SeisProfileItem::VarDensity);
}

void SeisProfileView::waveVarArea()
{
    mSeisItem->setDisplayMode(SeisProfileItem::WaveVarArea);
}

void SeisProfileView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    resetMargins();
    updateCoordinate();
}

void SeisProfileView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    updateCoordinate();
}

void SeisProfileView::updateCoordinate()
{
    QRect viewportRect = this->viewport()->rect();
    QRectF viewportRectFInScene = this->mapToScene(viewportRect).boundingRect();

    QwtLinearScaleEngine scaleEngine;
    double minX = viewportRectFInScene.left();
    double maxX = viewportRectFInScene.right();
    topScale->setScaleDiv(scaleEngine.divideScale(minX, maxX, 10, 5, 0));

    double minY = viewportRectFInScene.top();
    double maxY = viewportRectFInScene.bottom();
    leftScale->setScaleDiv(scaleEngine.divideScale(maxY, minY, 20, 5, 0));
    rightScale->setScaleDiv(scaleEngine.divideScale(maxY, minY, 20, 5, 0));
}

void SeisProfileView::resetMargins()
{
    QRect viewportRect = this->viewport()->rect();
    topScale->setGeometry(QRect(0, 0, viewLeftMarginWidth+viewportRect.width()+viewRightMarginWidth, viewTopMarginHeight));
    leftScale->setGeometry(QRect(0, 0, viewLeftMarginWidth, viewTopMarginHeight+viewportRect.height()+viewBotMarginHeight));
    rightScale->setGeometry(QRect(viewLeftMarginWidth+viewportRect.width(), 0, viewRightMarginWidth, viewTopMarginHeight+viewportRect.height()+viewBotMarginHeight));
}

void SeisProfileView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        fitInView(this->sceneRect());
    }

    QGraphicsView::mouseDoubleClickEvent(event);
}

void SeisProfileView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    QPointF posToScene = this->mapToScene(pos);
    topScaleDraw->setMousePosition(posToScene.x());
    leftScale->setMousePosition(posToScene);
    update();

    QGraphicsView::mouseMoveEvent(event);
}

void SeisProfileView::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8.0;
    int numSteps = numDegrees / 15.0;
    zoom(numSteps*0.1+1.0);
}

void SeisProfileView::zoomHorizontal(float factor)
{
    scale(factor, 1.0);
    updateCoordinate();
}

void SeisProfileView::zoomVertical(float factor)
{
    scale(1.0, factor);
    updateCoordinate();
}

void SeisProfileView::zoom(float factor)
{
    scale(factor, factor);
    updateCoordinate();
}
