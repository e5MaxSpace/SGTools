#ifndef COLORCANVASPICKER_H
#define COLORCANVASPICKER_H

#include <QObject>
#include <QVector>
#include <QPointF>
#include <QColor>

class QPoint;
class QwtPlot;
class QwtPlotCurve;

class ColorCanvasPicker: public QObject
{
    Q_OBJECT

public:
    explicit ColorCanvasPicker( QwtPlot *plot );
    virtual bool eventFilter( QObject *object, QEvent *event );

    virtual bool event( QEvent *ev );

signals:
    void moveColorFinished(const QVector<QPointF> &);
    void pickColorFinished(int index, const QColor &c);

private:
    void select( const QPoint &pos );
    void remove( const QPoint &pos );
    void move( const QPoint &pos );
    void moveBy( int dx, int dy );
    void pick( const QPoint &pos );
    void release();

    void showCursor( bool enable );
    void shiftPointCursor( bool up );
    void shiftCurveCursor( bool up );

    QwtPlot *plot();
    const QwtPlot *plot() const;

    QwtPlotCurve *d_selectedCurve;
    int d_selectedPoint;
};

#endif // COLORCANVASPICKER_H
