#ifndef HINTMOUSESCALEWIDGET_H
#define HINTMOUSESCALEWIDGET_H

#include <qwt_scale_widget.h>

class HintMouseScaleWidget : public QwtScaleWidget
{
    Q_OBJECT

public:
    explicit HintMouseScaleWidget(QWidget *parent = 0);
    explicit HintMouseScaleWidget(QwtScaleDraw::Alignment align, QWidget *parent = 0 );

    bool isEnabledMouseHint() const;
    void setEnabledMouseHint(bool on);

    void setMousePosition(const QPointF &mousePos);

    QColor mouseLineColor() const;
    void setMouseLineColor(const QColor &color);

protected:
    virtual void paintEvent( QPaintEvent *event );

    void drawMouseLine( QPainter *p ) const;

private:
    bool mEnabledMouseHint;
    QColor mMouseLineColor;
    QPointF mMousePos;
};

#endif // HINTMOUSESCALEWIDGET_H
