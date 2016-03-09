#ifndef TRANSCANVASPICKER_H
#define TRANSCANVASPICKER_H

#include <QObject>
#include <QVector>
#include <QPointF>

class QPoint;
class QwtPlot;
class QwtPlotCurve;

class TransCanvasPicker: public QObject
{
    Q_OBJECT

public:
    explicit TransCanvasPicker( QwtPlot *plot );
    virtual bool eventFilter( QObject *object, QEvent *event );

    virtual bool event( QEvent *ev );

signals:
    void pointChanged(const QVector<QPointF> &);

private:
    void select( const QPoint &pos );
    void remove( const QPoint &pos );
    void move( const QPoint &pos );
    void moveBy( int dx, int dy );

    void release();

    void showCursor( bool enable );
    void shiftPointCursor( bool up );
    void shiftCurveCursor( bool up );

    QwtPlot *plot();
    const QwtPlot *plot() const;

    QwtPlotCurve *d_selectedCurve;
    int d_selectedPoint;
};

#endif // TRANSCANVASPICKER_H
