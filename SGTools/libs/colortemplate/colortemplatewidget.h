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
 * @brief ��ɫֵ��͸���ȵ�����岿����
 *
 */
class COLORTEMPLATE_EXPORT ColorTemplateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ColorTemplateWidget(QWidget *parent = 0);

    /**
     * @brief ����X�����ʼ����
     *
     * @param interval
     */
    void setXInterval(const QwtInterval &interval);

    /**
     * @brief ����ɫ������
     *
     * @param colorData
     */
    void setCurrentColor(const ColorData &colorData);

    /**
     * @brief �������ݷֲ�ͳ�ƵĽ��
     *
     * @param counts
     */
    void setDistribution(const QVector<QwtIntervalSample> &counts);

    /**
     * @brief ���ص�ǰ��ɫ������
     *
     * @return ColorData
     */
    ColorData getCurrentColor() const;

    /**
     * @brief ��תɫ�꣨���ݣ�
     *
     */
    void reverseColor();

signals:
    /**
     * @brief ����ɫ�����˱仯��������ź�
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
