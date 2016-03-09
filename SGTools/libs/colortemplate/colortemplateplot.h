#ifndef COLORTEMPLATEPLOT_H
#define COLORTEMPLATEPLOT_H

#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_samples.h>

class QwtPlotCurve;
class ColorTemplateWidget;
class ColorTemplatePlot : public QwtPlot
{
    Q_OBJECT

public:
    explicit ColorTemplatePlot(QWidget *parent);

    void repaintGraphics();

signals:
    void colorChanged(const QGradientStops &stops);

private slots:
    void handleColorChange(const QVector<QPointF> &points);
    void updateColorOfPoint(int index, const QColor &c);

private:
    QwtPlotCurve *mColorPosCurve;

    ColorTemplateWidget *parentWidget();
    const ColorTemplateWidget *parentWidget() const;
};

#endif // COLORTEMPLATEPLOT_H
