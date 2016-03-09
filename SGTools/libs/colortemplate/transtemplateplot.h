#ifndef TRANSTEMPLATEPLOT_H
#define TRANSTEMPLATEPLOT_H

#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_samples.h>

#include "colordata.h"

class QwtPlotCurve;
class QwtPlotHistogram;
class ColorTemplateWidget;
class TransTemplatePlot : public QwtPlot
{
    Q_OBJECT

public:
    explicit TransTemplatePlot(QWidget *parent);

    void setDistribution(const QVector<QwtIntervalSample> &counts);

    void repaintGraphics();

signals:
    void transChanged(const QTransStops &trans);

private slots:
    void handleTransChange(const QVector<QPointF> &points);

private:
    QwtPlotHistogram *mHistogramPlot;
    QwtPlotCurve *mTransPosCurve;

    ColorTemplateWidget *parentWidget();
    const ColorTemplateWidget *parentWidget() const;
};

#endif // TRANSTEMPLATEPLOT_H
