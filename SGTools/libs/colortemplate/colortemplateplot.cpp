#include "colortemplateplot.h"

#include <qwt_scale_div.h>
#include <qwt_scale_widget.h>
#include <qwt_color_map.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include "colorcanvaspicker.h"
#include "colortemplatewidget.h"

ColorTemplatePlot::ColorTemplatePlot(QWidget *parent) :
    QwtPlot(parent)
{
    this->enableAxis(xTop, false);
    this->enableAxis(yLeft, false);
    this->enableAxis(yRight, false);

    mColorPosCurve = new QwtPlotCurve();
    setAxisScale( QwtPlot::yLeft,  0.0, 10.0 );
    mColorPosCurve->setSymbol( new QwtSymbol( QwtSymbol::Cross, QBrush(Qt::red), QPen(Qt::red), QSize( 8, 8 ) ) );
    mColorPosCurve->attach(this);

    this->axisWidget(xBottom)->setColorBarEnabled( true );
    this->axisWidget(xBottom)->setColorBarWidth( 20 );

    ColorCanvasPicker *canvasPicker = new ColorCanvasPicker( this );
    connect(canvasPicker, SIGNAL(moveColorFinished(QVector<QPointF>)), SLOT(handleColorChange(QVector<QPointF>)));
    connect(canvasPicker, SIGNAL(pickColorFinished(int, QColor)), SLOT(updateColorOfPoint(int, QColor)));
}

void ColorTemplatePlot::handleColorChange(const QVector<QPointF> &points)
{
    double minValue = parentWidget()->mXInterval.minValue();
    double maxValue = parentWidget()->mXInterval.maxValue();
    QGradientStops stops;
    foreach (const QPointF &p, points)
    {
        double x = (p.x() - minValue)/(maxValue - minValue);
        if (x < 0.0)
            x = 0.0;
        if (x > 1.0)
            x = 1.0;
        stops.push_back(QPair<qreal, QColor>(x, parentWidget()->mColorData.color(minValue, maxValue, p.x())));
    }

    parentWidget()->mColorData.setColorStops(stops);
    this->axisWidget(xBottom)->setColorMap(parentWidget()->mXInterval, parentWidget()->mColorData.toLinearColorMap());

    emit colorChanged(stops);
}

void ColorTemplatePlot::updateColorOfPoint(int index, const QColor &c)
{
    parentWidget()->mColorData.updateColorOfPoint(index, c);
    this->axisWidget(xBottom)->setColorMap(parentWidget()->mXInterval, parentWidget()->mColorData.toLinearColorMap());
}

void ColorTemplatePlot::repaintGraphics()
{
    double minValue = parentWidget()->mXInterval.minValue();
    double maxValue = parentWidget()->mXInterval.maxValue();

    this->setAxisScale(xBottom, minValue, maxValue);
    this->axisWidget(xBottom)->setColorMap(parentWidget()->mXInterval, parentWidget()->mColorData.toLinearColorMap());
    mColorPosCurve->setSamples(parentWidget()->mColorData.colorStops(minValue, maxValue));
    this->replot();
}

ColorTemplateWidget *ColorTemplatePlot::parentWidget()
{
    return qobject_cast<ColorTemplateWidget *>( parent() );
}

const ColorTemplateWidget *ColorTemplatePlot::parentWidget() const
{
    return qobject_cast<const ColorTemplateWidget *>( parent() );
}
