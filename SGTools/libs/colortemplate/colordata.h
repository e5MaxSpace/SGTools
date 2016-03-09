#ifndef COLORDATA_H
#define COLORDATA_H

#include "colortemplate_global.h"

#include <QImage>
#include <QGradientStops>

class QwtLinearColorMap;

/**
 * @brief 表征一个透明度点：QPair<qreal[0,1], int[0,255]>
 *
 */
typedef QPair<qreal, int> QTransStop;

/**
 * @brief  表征透明度列表
 *
 */
typedef QVector<QTransStop> QTransStops;

/**
 * @brief  表征一个颜色模板数据（色标对象）
 *
 */
class COLORTEMPLATE_EXPORT ColorData
{
public:
    /**
     * @brief 从fileName指定的文本文件中生成（创建）一个ColorData
     *
     * @param fileName
     * @return ColorData
     */
    static ColorData fromColorFile(const QString &fileName);

public:
    /**
     * @brief 构建一个名称为colorName的色标对象， 此时色标是无效的
     *
     * @param colorName
     */
    explicit ColorData(const QString &colorName = QString::null);

    /**
     * @brief 返回色标对象是否为有效色标
     *
     * @return bool
     */
    bool isValid() const;

    /**
     * @brief 返回颜色名称
     *
     * @return QString
     */
    QString getColorName() const;

    /**
     * @brief 设置（即更改）颜色名称
     *
     * @param colorName
     */
    void setColorName(const QString &colorName);

    /**
     * @brief 重载“相等”运算符，给色标赋予相等语义
     *
     * @param rhs 色标对象
     * @return bool
     */
    bool operator==(const ColorData& rhs) const;

    /**
     * @brief 重载“不相等”运算符，给色标赋予不相等语义
     *
     * @param rhs 色标对象
     * @return bool
     */
    bool operator!=(const ColorData& rhs) const;

    /**
     * @brief 设置色标的颜色点列表
     *
     * @param stops
     */
    void setColorStops(const QGradientStops &stops);

    /**
     * @brief 更新（即替换）色标的指定索引位置的颜色值
     *
     * @param index
     * @param color
     */
    void updateColorOfPoint(int index, const QColor &color);

    /**
     * @brief 设置色标的透明度点列表
     *
     * @param stops
     */
    void setTransStops(const QTransStops &stops);

    /**
     * @brief 反转色标
     *
     */
    void reverse();

    /**
     * @brief 保存色表对象到fileName指定的文件
     *
     * @param fileName
     * @return bool
     */
    bool saveToFile(const QString &fileName) const;

    /**
     * @brief 根据输入的宽度w和高度h生成一张QImage图片
     *
     * @param w
     * @param h
     * @return QImage
     */
    QImage toImage(int w = 255, int h = 20) const;

    /**
     * @brief 返回一个指向QwtLinearColorMap对象的指针
     *
     * @return QwtLinearColorMap
     */
    QwtLinearColorMap *toLinearColorMap() const;

    /**
     * @brief 将颜色点列表[0, 1]映射到[minValue, maxValue]上，并返回一个新的列表
     *
     * @param minValue
     * @param maxValue
     * @return QVector<QPointF>
     */
    QVector<QPointF> colorStops(double minValue, double maxValue) const;

    /**
     * @brief 使用IGeoSeis的算法生成256个颜色值
     *
     * @return QVector<QRgb>
     */
    QVector<QRgb> generateColorValues_igeoseis() const;

    /**
     * @brief 使用Qwt的算法生成256个颜色值
     *
     * @return QVector<QRgb>
     */
    QVector<QRgb> generateColorValues_qwt() const;

    /**
     * @brief 根据输入的区间[minValue, maxValue]， 计算值value对应点的颜色值
     *
     * @param minValue
     * @param maxValue
     * @param value
     * @return QColor
     */
    QColor color(double minValue, double maxValue, double value) const;

    /**
     * @brief 将透明度点列表[0, 1]映射到[minValue, maxValue]上，并返回一个新的列表
     *
     * @param minValue
     * @param maxValue
     * @return QVector<QPointF>
     */
    QVector<QPointF> transStops(double minValue, double maxValue) const;

    /**
     * @brief 生成256个透明度值
     *
     * @return QVector<quint8>
     */
    QVector<quint8> generateTransValues() const;

private:
    QString         m_colorName;
    QGradientStops  m_colorStops;
    QTransStops     m_transStops;
};

#endif // COLORDATA_H
