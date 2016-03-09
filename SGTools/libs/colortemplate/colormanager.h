#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include "colortemplate_global.h"

#include "colordata.h"

/**
 * @brief 颜色模板管理类，为一个单例对象
 *
 */
class COLORTEMPLATE_EXPORT ColorManager
{
public:
    explicit ColorManager();

    /**
     * @brief ColorManager单例对象全局访问点
     *
     * @return ColorManager
     */
    static ColorManager &getInstance();

    /**
     * @brief 设置颜色模板文件的存放路径，必须设置！
     * e.g. ColorManager::getInstance().setColorTemplatePath("E:/buildQIGG/bin/colortables");
     *
     * @param path
     */
    void setColorTemplatePath(const QString &path);

    /**
     * @brief 获取颜色模板文件的存放路径
     *
     * @return QString
     */
    QString getColorTemplatePath() const;

    /**
     * @brief 判断是否存在名称为colorName的色标
     *
     * @param colorName
     * @return bool
     */
    bool isExist(const QString &colorName) const;

    /**
     * @brief 根据色标名称colorName得到色标数据
     *
     * @param colorName
     * @return ColorData
     */
    ColorData getColorData(const QString &colorName) const;

    /**
     * @brief 获取所有色标名称的列表
     *
     * @return QStringList
     */
    QStringList getColorNames() const;

    /**
     * @brief 创建（即新建）一个名称为colorName的色标（默认为黑白渐变）
     *
     * @param colorName
     * @return bool
     */
    bool createColorData(const QString &colorName) const;

    /**
     * @brief 保存色标数据
     *
     * @param colorData
     * @return bool
     */
    bool saveColorData(const ColorData &colorData) const;

    /**
     * @brief 删除色标（文件）
     *
     * @param colorName
     * @return bool
     */
    bool removeColorData(const QString &colorName) const;

private:
    QString m_colorTemplatePath;

    QString getColorTemplateFileName(const QString &colorName) const;
};

#endif // COLORMANAGER_H
