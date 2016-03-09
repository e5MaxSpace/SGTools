#include "colormanager.h"

#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDebug>

Q_GLOBAL_STATIC(ColorManager, colorManager)

ColorManager::ColorManager()
{
}

ColorManager &ColorManager::getInstance()
{
    return *colorManager();
}

void ColorManager::setColorTemplatePath(const QString &path)
{
    m_colorTemplatePath = path;
}

QString ColorManager::getColorTemplatePath() const
{
    return m_colorTemplatePath;
}

bool ColorManager::isExist(const QString &colorName) const
{
    return getColorNames().contains(colorName);
}

ColorData ColorManager::getColorData(const QString &colorName) const
{
    const QString fileName = getColorTemplateFileName(colorName);
    return ColorData::fromColorFile(fileName);
}

QStringList ColorManager::getColorNames() const
{
    QDir dir(m_colorTemplatePath);
    QStringList fileNames = dir.entryList(QStringList()<<"*.col");
    QStringList colorNames;
    foreach (const QString &fileName, fileNames)
    {
        colorNames.append(QFileInfo(fileName).baseName());
    }

    return colorNames;
}

bool ColorManager::createColorData(const QString &colorName) const
{
    ColorData colorData(colorName);
    colorData.setColorStops(QGradientStops()<<QPair<qreal, QColor>(0.0, QColor(Qt::white))<<QPair<qreal, QColor>(1, QColor(Qt::black)));
    colorData.setTransStops(QTransStops()<<QPair<qreal, int>(0.0, 255)<<QPair<qreal, int>(1.0, 255));

    return saveColorData(colorData);
}

bool ColorManager::saveColorData(const ColorData &colorData) const
{
    const QString fileName = getColorTemplateFileName(colorData.getColorName());
    return colorData.saveToFile(fileName);
}

bool ColorManager::removeColorData(const QString &colorName) const
{
    const QString fileName = getColorTemplateFileName(colorName);
    return QFile::remove(fileName);
}

QString ColorManager::getColorTemplateFileName(const QString &colorName) const
{
    if (m_colorTemplatePath.isEmpty())
    {
        qWarning()<<"error! no color template path is specified.";
    }

    QString colorTemplatePath(m_colorTemplatePath);
    if (!colorTemplatePath.endsWith('/'))
        colorTemplatePath += '/';

    return (colorTemplatePath + colorName + ".col");
}
