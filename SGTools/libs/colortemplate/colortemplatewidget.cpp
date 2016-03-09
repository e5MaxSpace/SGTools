#include "colortemplatewidget.h"

#include <QVBoxLayout>

#include <qwt_scale_widget.h>

#include "transtemplateplot.h"
#include "colortemplateplot.h"

ColorTemplateWidget::ColorTemplateWidget(QWidget *parent) :
    QWidget(parent)
{
    mTransPlot = new TransTemplatePlot(this);
    mTransPlot->setCanvasBackground(Qt::white);

    mColorPlot = new ColorTemplatePlot(this);
    qobject_cast<QFrame*>(mColorPlot->canvas())->setFrameShape(QFrame::NoFrame);
    mColorPlot->setFixedHeight(60);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addWidget(mTransPlot);
    vLayout->addWidget(mColorPlot);

    this->setLayout(vLayout);

    connect(mTransPlot, SIGNAL(transChanged(QTransStops)), SLOT(handleTransChange(QTransStops)));
    connect(mColorPlot, SIGNAL(colorChanged(QGradientStops)), SLOT(handleColorChange(QGradientStops)));
}

void ColorTemplateWidget::setXInterval(const QwtInterval &interval)
{
    if (interval != mXInterval)
    {
        mXInterval = interval;
        repaintGraphics();
    }
}

void ColorTemplateWidget::setCurrentColor(const ColorData &colorData)
{
    if (colorData != mColorData)
    {
        mColorData = colorData;
        repaintGraphics();
    }
}

void ColorTemplateWidget::setDistribution(const QVector<QwtIntervalSample> &counts)
{
    mTransPlot->setDistribution(counts);
}

ColorData ColorTemplateWidget::getCurrentColor() const
{
    return mColorData;
}

void ColorTemplateWidget::handleTransChange(const QTransStops &points)
{
    mColorData.setTransStops(points);
    emit colorDataChanged(mColorData);
}

void ColorTemplateWidget::handleColorChange(const QGradientStops &points)
{
    mColorData.setColorStops(points);
    emit colorDataChanged(mColorData);
}

void ColorTemplateWidget::reverseColor()
{
    mColorData.reverse();

    setCurrentColor(mColorData);

    emit colorDataChanged(mColorData);
}

void ColorTemplateWidget::repaintGraphics()
{
    mTransPlot->repaintGraphics();
    mColorPlot->repaintGraphics();
}
