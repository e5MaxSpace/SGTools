#include "hintmousescalewidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>

HintMouseScaleWidget::HintMouseScaleWidget(QWidget *parent) :
    QwtScaleWidget(parent), mEnabledMouseHint(true)
{
}

HintMouseScaleWidget::HintMouseScaleWidget(QwtScaleDraw::Alignment align, QWidget *parent) :
    QwtScaleWidget(align, parent), mEnabledMouseHint(true)
{

}

bool HintMouseScaleWidget::isEnabledMouseHint() const
{
    return mEnabledMouseHint;
}

void HintMouseScaleWidget::setEnabledMouseHint(bool on)
{
    if (mEnabledMouseHint != on)
    {
        mEnabledMouseHint = on;
        update();
    }
}

void HintMouseScaleWidget::setMousePosition(const QPointF &mousePos)
{
    if (mMousePos != mousePos)
    {
        mMousePos = mousePos;
        update();
    }
}

QColor HintMouseScaleWidget::mouseLineColor() const
{
    return mMouseLineColor;
}

void HintMouseScaleWidget::setMouseLineColor(const QColor &color)
{
    if (mMouseLineColor != color)
    {
        mMouseLineColor = color;
        update();
    }
}

void HintMouseScaleWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );

    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    draw( &painter );

    if (mEnabledMouseHint)
        drawMouseLine( &painter );
}

void HintMouseScaleWidget::drawMouseLine(QPainter *p) const
{
    QRect r = this->contentsRect();
    QLineF lineF;
    if ( this->scaleDraw()->orientation() == Qt::Horizontal )
    {
        QPointF pos(this->scaleDraw()->labelPosition(mMousePos.x()));
        lineF.setP1(pos);
        lineF.setP2(QPointF(pos.x(), r.top()));
    }
    else
    {
        QPointF pos(this->scaleDraw()->labelPosition(mMousePos.y()));
        lineF.setP1(pos);
        lineF.setP2(QPointF(r.left(), pos.y()));
    }

    p->save();

    if (mMouseLineColor.isValid())
        p->setPen(mMouseLineColor);

    p->drawLine(lineF);

    p->restore();
}
