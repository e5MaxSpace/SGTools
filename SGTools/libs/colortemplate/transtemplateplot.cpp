#include "transtemplateplot.h"

#include <qwt_scale_div.h>
#include <qwt_scale_widget.h>
#include <qwt_color_map.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include "transcanvaspicker.h"
#include "colortemplatewidget.h"

TransTemplatePlot::TransTemplatePlot(QWidget *parent) :
    QwtPlot(parent)
{
    this->enableAxis(xTop, false);
    this->enableAxis(xBottom, false);
    this->enableAxis(yLeft, false);
    this->enableAxis(yRight, false);

    mHistogramPlot = new QwtPlotHistogram();
    QColor bg( Qt::blue );
    bg.setAlpha( 150 );
    mHistogramPlot->setPen(QPen(bg));
    mHistogramPlot->setBrush( QBrush( bg ) );
    mHistogramPlot->setStyle( QwtPlotHistogram::Columns );
    mHistogramPlot->attach(this);

    mTransPosCurve = new QwtPlotCurve();
    setAxisScale( QwtPlot::yRight,  0.0, 255.0 );
    mTransPosCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    mTransPosCurve->setYAxis(yRight);
    mTransPosCurve->setSymbol( new QwtSymbol( QwtSymbol::Ellipse,
                                              QBrush(QColor(191, 191, 191, 127)),
                                              QPen(QColor(187, 187, 187, 191), 1),
                                              QSize(10, 10) ) );
    mTransPosCurve->attach(this);

    TransCanvasPicker *canvasPicker = new TransCanvasPicker( this );
    connect(canvasPicker, SIGNAL(pointChanged(QVector<QPointF>)), SLOT(handleTransChange(QVector<QPointF>)));
}

void TransTemplatePlot::setDistribution(const QVector<QwtIntervalSample> &counts)
{
    mHistogramPlot->setSamples(counts);
    this->replot();
}

void TransTemplatePlot::repaintGraphics()
{
    double minValue = parentWidget()->mXInterval.minValue();
    double maxValue = parentWidget()->mXInterval.maxValue();
    qDebug()<<"minValue="<<minValue<<" maxValue="<<maxValue;
    this->setAxisScale(xBottom, minValue, maxValue);
    mTransPosCurve->setSamples(parentWidget()->mColorData.transStops(minValue, maxValue));
    this->replot();
}

void TransTemplatePlot::handleTransChange(const QVector<QPointF> &points)
{
    double minValue = parentWidget()->mXInterval.minValue();
    double maxValue = parentWidget()->mXInterval.maxValue();
    QTransStops trans;
    foreach (const QPointF &p, points)
    {
        double x = (p.x() - minValue)/(maxValue - minValue);
        if (x < 0.0)
            x = 0.0;
        if (x > 1.0)
            x = 1.0;
        trans.push_back(QPair<qreal, int>(x, qRound(p.y())));
    }

    emit transChanged(trans);
}

ColorTemplateWidget *TransTemplatePlot::parentWidget()
{
    return qobject_cast<ColorTemplateWidget *>( parent() );
}

const ColorTemplateWidget *TransTemplatePlot::parentWidget() const
{
    return qobject_cast<const ColorTemplateWidget *>( parent() );
}
