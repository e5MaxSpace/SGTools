#include <qapplication.h>
#include <qevent.h>
#include <qwhatsthis.h>
#include <qpainter.h>
#include <qwt_plot.h>
#include <qwt_symbol.h>
#include <qwt_scale_map.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include "transcanvaspicker.h"

static bool lessPoint(const QPointF &p1, const QPointF &p2)
{
    return p1.x() < p2.x();
}

static bool nearCurve(const QwtPlotCurve *curve, const QPointF &point)
{
    // TODO: 该函数目的是 判断点point是否在曲线curve的附近，但算法还不够理想
    QPolygonF curvePolygon;
    for (int i=0; i<static_cast<int>(curve->dataSize()); ++i)
    {
        curvePolygon.push_back(curve->sample(i));
    }

    //    QPainterPath aPainterPath;
    //    aPainterPath.addPolygon(curvePolygon);

    //    QPainterPathStroker aPathStroker;
    //    aPathStroker.setMiterLimit(2);
    //    aPathStroker.setCurveThreshold(200);
    //    aPainterPath = aPathStroker.createStroke(aPainterPath);

    //    return aPainterPath.contains(point);

    return curvePolygon.containsPoint(point, Qt::WindingFill);
}

TransCanvasPicker::TransCanvasPicker( QwtPlot *plot ):
    QObject( plot ),
    d_selectedCurve( NULL ),
    d_selectedPoint( -1 )
{
    QwtPlotCanvas *canvas = qobject_cast<QwtPlotCanvas *>( plot->canvas() );
    canvas->installEventFilter( this );

    // We want the focus, but no focus rect. The
    // selected point will be highlighted instead.

    canvas->setFocusPolicy( Qt::StrongFocus );
#ifndef QT_NO_CURSOR
    canvas->setCursor( Qt::PointingHandCursor );
#endif
    canvas->setFocusIndicator( QwtPlotCanvas::ItemFocusIndicator );
    canvas->setFocus();

    //    const char *text =
    //            "All points can be moved using the left mouse button "
    //            "or with these keys:\n\n"
    //            "- Up:\t\tSelect next curve\n"
    //            "- Down:\t\tSelect previous curve\n"
    //            "- Left, ??\tSelect next point\n"
    //            "- Right, ??\tSelect previous point\n"
    //            "- 7, 8, 9, 4, 6, 1, 2, 3:\tMove selected point";
    //    canvas->setWhatsThis( text );

    shiftCurveCursor( true );
}

QwtPlot *TransCanvasPicker::plot()
{
    return qobject_cast<QwtPlot *>( parent() );
}

const QwtPlot *TransCanvasPicker::plot() const
{
    return qobject_cast<const QwtPlot *>( parent() );
}

bool TransCanvasPicker::event( QEvent *ev )
{
    if ( ev->type() == QEvent::User )
    {
        showCursor( true );
        return true;
    }
    return QObject::event( ev );
}

bool TransCanvasPicker::eventFilter( QObject *object, QEvent *event )
{
    if ( plot() == NULL || object != plot()->canvas() )
        return false;

    switch( event->type() )
    {
    case QEvent::FocusIn:
    {
        showCursor( true );
        break;
    }
    case QEvent::FocusOut:
    {
        showCursor( false );
        break;
    }
    case QEvent::Paint:
    {
        QApplication::postEvent( this, new QEvent( QEvent::User ) );
        break;
    }
    case QEvent::MouseButtonPress:
    {
        const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
        if (mouseEvent->button() == Qt::LeftButton)
            select( mouseEvent->pos() );
        else
            remove( mouseEvent->pos() );
        return true;
    }
        //    case QEvent::MouseButtonRelease:
        //    {
        //        //const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
        //        release( /*mouseEvent->pos()*/ );
        //        return true;
        //    }
    case QEvent::MouseMove:
    {
        const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
        move( mouseEvent->pos() );
        return true;
    }
    case QEvent::KeyPress:
    {
        const QKeyEvent *keyEvent = static_cast<QKeyEvent *>( event );

        const int delta = 5;
        switch( keyEvent->key() )
        {
        case Qt::Key_Up:
        {
            shiftCurveCursor( true );
            return true;
        }
        case Qt::Key_Down:
        {
            shiftCurveCursor( false );
            return true;
        }
        case Qt::Key_Right:
        case Qt::Key_Plus:
        {
            if ( d_selectedCurve )
                shiftPointCursor( true );
            else
                shiftCurveCursor( true );
            return true;
        }
        case Qt::Key_Left:
        case Qt::Key_Minus:
        {
            if ( d_selectedCurve )
                shiftPointCursor( false );
            else
                shiftCurveCursor( true );
            return true;
        }

            // The following keys represent a direction, they are
            // organized on the keyboard.

        case Qt::Key_1:
        {
            moveBy( -delta, delta );
            break;
        }
        case Qt::Key_2:
        {
            moveBy( 0, delta );
            break;
        }
        case Qt::Key_3:
        {
            moveBy( delta, delta );
            break;
        }
        case Qt::Key_4:
        {
            moveBy( -delta, 0 );
            break;
        }
        case Qt::Key_6:
        {
            moveBy( delta, 0 );
            break;
        }
        case Qt::Key_7:
        {
            moveBy( -delta, -delta );
            break;
        }
        case Qt::Key_8:
        {
            moveBy( 0, -delta );
            break;
        }
        case Qt::Key_9:
        {
            moveBy( delta, -delta );
            break;
        }
        default:
            break;
        }
    }
    default:
        break;
    }

    return QObject::eventFilter( object, event );
}

// Select the point at a position. If there is no point
// deselect the selected point
void TransCanvasPicker::select( const QPoint &pos )
{
    QwtPlotCurve *curve = NULL;
    double dist = 10e10;
    int index = -1;
    bool insertPoint = false;

    const QwtPlotItemList& itmList = plot()->itemList();
    for ( QwtPlotItemIterator it = itmList.begin();
          it != itmList.end(); ++it )
    {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            QwtPlotCurve *c = static_cast<QwtPlotCurve *>( *it );

            double d;
            int idx = c->closestPoint( pos, &d );
            if ( d < dist )
            {
                curve = c;
                index = idx;
                dist = d;
            }

            double x = plot()->invTransform( c->xAxis(), pos.x() );
            double y = plot()->invTransform( c->yAxis(), pos.y() );
            insertPoint = nearCurve(c, QPointF(x, y));
        }
    }

    showCursor( false );
    d_selectedCurve = NULL;
    d_selectedPoint = -1;

    if (curve == NULL)
        return;

    if ( dist < 10 ) // 10 pixels tolerance
    {
        d_selectedCurve = curve;
        d_selectedPoint = index;
        showCursor( true );
        return;
    }

    if (insertPoint)
    {
        double x = plot()->invTransform( curve->xAxis(), pos.x() );
        double y = plot()->invTransform( curve->yAxis(), pos.y() );
        QVector<QPointF> curvePoints;
        for (int i=0; i<static_cast<int>(curve->dataSize()); ++i)
        {
            curvePoints.push_back(curve->sample(i));
        }
        curvePoints.push_back(QPointF(x, y));
        qSort(curvePoints.begin(), curvePoints.end(), lessPoint);
        curve->setSamples(curvePoints);
        qDebug()<<"insert-curvePoints="<<curvePoints;
        d_selectedCurve = curve;
        d_selectedPoint = curvePoints.indexOf(QPointF(x, y));
        qDebug()<<"insert-d_selectedPoint="<<d_selectedPoint;
        showCursor( true );
    }
}

void TransCanvasPicker::remove(const QPoint &pos)
{
    QwtPlotCurve *curve = NULL;
    double dist = 10e10;
    int index = -1;

    const QwtPlotItemList& itmList = plot()->itemList();
    for ( QwtPlotItemIterator it = itmList.begin();
          it != itmList.end(); ++it )
    {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            QwtPlotCurve *c = static_cast<QwtPlotCurve *>( *it );

            double d;
            int idx = c->closestPoint( pos, &d );
            if ( d < dist )
            {
                curve = c;
                index = idx;
                dist = d;
            }
        }
    }

    showCursor( false );
    d_selectedCurve = NULL;
    d_selectedPoint = -1;

    if ( curve && dist < 10 ) // 10 pixels tolerance
    {
        QVector<QPointF> curvePoints;
        for (int i=0; i<static_cast<int>(curve->dataSize()); ++i)
        {
            curvePoints.push_back(curve->sample(i));
        }
        curvePoints.remove(index);
        curve->setSamples(curvePoints);

        d_selectedCurve = curve;
        d_selectedPoint = index - 1;
        showCursor( true );
        plot()->replot();
    }
}

// Move the selected point
void TransCanvasPicker::moveBy( int dx, int dy )
{
    if ( dx == 0 && dy == 0 )
        return;

    if ( !d_selectedCurve )
        return;

    const QPointF sample =
            d_selectedCurve->sample( d_selectedPoint );

    const double x = plot()->transform(
                d_selectedCurve->xAxis(), sample.x() );
    const double y = plot()->transform(
                d_selectedCurve->yAxis(), sample.y() );

    move( QPoint( qRound( x + dx ), qRound( y + dy ) ) );
}

void TransCanvasPicker::release()
{
    QVector<QPointF> curvePoints;
    for ( int i = 0; i < static_cast<int>( d_selectedCurve->dataSize() ); i++ )
    {
        curvePoints.push_back(d_selectedCurve->sample( i ));
    }
    qDebug()<<"curvePoints="<<curvePoints;
    emit pointChanged(curvePoints);
}

// Move the selected point
void TransCanvasPicker::move( const QPoint &pos )
{
    if (!plot()->canvas()->rect().contains(pos, true))
        return;

    if ( !d_selectedCurve )
        return;

    if (d_selectedPoint==0 || d_selectedPoint==static_cast<int>( d_selectedCurve->dataSize()-1 ))
        return;

    QVector<double> xData( d_selectedCurve->dataSize() );
    QVector<double> yData( d_selectedCurve->dataSize() );
    for ( int i = 0; i < static_cast<int>( d_selectedCurve->dataSize() ); i++ )
    {
        const QPointF sample = d_selectedCurve->sample( i );
        if ( i == d_selectedPoint )
        {
            xData[i] = /*sample.x();*/ plot()->invTransform( d_selectedCurve->xAxis(), pos.x() );
            yData[i] = plot()->invTransform( d_selectedCurve->yAxis(), pos.y() );
        }
        else
        {
            xData[i] = sample.x();
            yData[i] = sample.y();
        }
    }
    d_selectedCurve->setSamples( xData, yData );

    /*
       Enable QwtPlotCanvas::ImmediatePaint, so that the canvas has been
       updated before we paint the cursor on it.
     */
    QwtPlotCanvas *plotCanvas =
            qobject_cast<QwtPlotCanvas *>( plot()->canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    plot()->replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );

    showCursor( true );
}

// Hightlight the selected point
void TransCanvasPicker::showCursor( bool showIt )
{
    if ( !d_selectedCurve )
        return;

    QwtSymbol *symbol = const_cast<QwtSymbol *>( d_selectedCurve->symbol() );

    const QBrush brush = symbol->brush();
    if ( showIt )
        symbol->setBrush( symbol->brush().color().dark( 180 ) );

    QwtPlotDirectPainter directPainter;
    directPainter.drawSeries( d_selectedCurve, d_selectedPoint, d_selectedPoint );

    if ( showIt )
        symbol->setBrush( brush ); // reset brush
}

// Select the next/previous curve
void TransCanvasPicker::shiftCurveCursor( bool up )
{
    QwtPlotItemIterator it;

    const QwtPlotItemList &itemList = plot()->itemList();

    QwtPlotItemList curveList;
    for ( it = itemList.begin(); it != itemList.end(); ++it )
    {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve )
            curveList += *it;
    }
    if ( curveList.isEmpty() )
        return;

    it = curveList.begin();

    if ( d_selectedCurve )
    {
        for ( it = curveList.begin(); it != curveList.end(); ++it )
        {
            if ( d_selectedCurve == *it )
                break;
        }
        if ( it == curveList.end() ) // not found
            it = curveList.begin();

        if ( up )
        {
            ++it;
            if ( it == curveList.end() )
                it = curveList.begin();
        }
        else
        {
            if ( it == curveList.begin() )
                it = curveList.end();
            --it;
        }
    }

    showCursor( false );
    d_selectedPoint = 0;
    d_selectedCurve = static_cast<QwtPlotCurve *>( *it );
    showCursor( true );
}

// Select the next/previous neighbour of the selected point
void TransCanvasPicker::shiftPointCursor( bool up )
{
    if ( !d_selectedCurve )
        return;

    int index = d_selectedPoint + ( up ? 1 : -1 );
    index = ( index + d_selectedCurve->dataSize() ) % d_selectedCurve->dataSize();

    if ( index != d_selectedPoint )
    {
        showCursor( false );
        d_selectedPoint = index;
        showCursor( true );
    }
}
