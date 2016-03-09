#ifndef SEISPROFILEVIEW_H
#define SEISPROFILEVIEW_H

#include <QGraphicsView>

#include "linecdpscaledraw.h"

class QGraphicsScene;
class QwtScaleWidget;
class HintMouseScaleWidget;
class SeisProfileData;
class SeisProfileItem;

/** @brief 显示地震剖面的视口类 */
class SeisProfileView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit SeisProfileView(QWidget *parent = 0);

    virtual ~SeisProfileView();

    const SeisProfileData& getSeisProfileData() { return m_datas; }
    void setSeisProfileData(const SeisProfileData &seisProfileData);

    void setWellPosition(const QList<WellPositionInScale> &wellPositions);

    void setColorMap(const QVector<QRgb> &vecColor);

    void varDensity();

    void waveVarArea();

public slots:
    void zoomHorizontal(float factor);

    void zoomVertical(float factor);

    void zoom(float factor);

protected:
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void scrollContentsBy(int dx, int dy);

private:
    void updateCoordinate();

    void resetMargins();

private:
    QGraphicsScene *mScene;
    SeisProfileItem *mSeisItem;

    SeisProfileData m_datas;

    LineCdpScaleDraw *topScaleDraw;
    QwtScaleWidget  *topScale;
    HintMouseScaleWidget  *leftScale;
    HintMouseScaleWidget  *rightScale;

    int viewTopMarginHeight;
    int viewBotMarginHeight;
    int viewLeftMarginWidth;
    int viewRightMarginWidth;
};

/** @brief 纵向标尺过滤器 */
class VScaleFilter : public QObject
{
    Q_OBJECT

public:
    explicit VScaleFilter(QObject *parent=0);
    virtual bool eventFilter(QObject *, QEvent *);

Q_SIGNALS:
    void scale(float factor);
};

#endif // SEISPROFILEVIEW_H
