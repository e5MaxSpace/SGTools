#ifndef COLORTEMPLATEWIDGET_H
#define COLORTEMPLATEWIDGET_H

#include "colortemplate_global.h"

#include <QWidget>

#include <qwt_interval.h>
#include <qwt_samples.h>

#include "colordata.h"

class TransTemplatePlot;
class ColorTemplatePlot;

/**
 * @brief 颜色值和透明度调节面板部件类
 *
 */
class COLORTEMPLATE_EXPORT ColorTemplateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ColorTemplateWidget(QWidget *parent = 0);

    /**
     * @brief 设置X轴的起始坐标
     *
     * @param interval
     */
    void setXInterval(const QwtInterval &interval);

    /**
     * @brief 设置色标数据
     *
     * @param colorData
     */
    void setCurrentColor(const ColorData &colorData);

    /**
     * @brief 设置数据分布统计的结果
     *
     * @param counts
     */
    void setDistribution(const QVector<QwtIntervalSample> &counts);

    /**
     * @brief 返回当前的色标数据
     *
     * @return ColorData
     */
    ColorData getCurrentColor() const;

    /**
     * @brief 反转色标（数据）
     *
     */
    void reverseColor();

signals:
    /**
     * @brief 当颜色发生了变化，发射此信号
     *
     * @param colorData
     */
    void colorDataChanged(const ColorData &colorData);

private slots:
    void handleTransChange(const QTransStops &points);

    void handleColorChange(const QGradientStops &points);

private:
    TransTemplatePlot *mTransPlot;
    ColorTemplatePlot *mColorPlot;

    friend class TransTemplatePlot;
    friend class ColorTemplatePlot;
    QwtInterval mXInterval;
    ColorData   mColorData;

    void repaintGraphics();
};

#endif // COLORTEMPLATEWIDGET_H
