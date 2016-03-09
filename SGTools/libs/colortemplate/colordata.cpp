#include "colordata.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QPainter>
#include <QDebug>

#include <qwt_color_map.h>

ColorData ColorData::fromColorFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
    {
        qWarning()<<"open file failed. fileName="<<fileName;
        return ColorData();
    }

    ColorData colorData;
    QGradientStops  colorStops;
    QTransStops     transStops;
    while (!file.atEnd())
    {
        QString data = file.readLine();
        QStringList listData = data.split(':');
        if (listData.size() != 2)
        {
            continue;
        }

        QStringList firstData = listData[0].split('.');
        if ( (firstData.size() == 2) && (firstData[1].trimmed() == "Name") )
        {
            const QString colorName = listData[1].trimmed();
            colorData.setColorName(colorName);
        }
        else if ( (firstData.size() == 3) && ("Value-Color" == firstData[1].trimmed()) )
        {
            QStringList colorStop = listData[1].split('`');
            if (5 != colorStop.size() && 4 != colorStop.size())
            {
                continue;
            }

            qreal percent = colorStop[0].toDouble();
            int r = colorStop[1].toInt();
            int g = colorStop[2].toInt();
            int b = colorStop[3].toInt();

            QGradientStop stop = QPair<qreal, QColor>(percent, QColor(r, g, b));
            colorStops.append(stop);
        }
        else if ( (3 == firstData.size()) && ("Transparency" == firstData[1].trimmed()) )
        {
            QStringList transStop = listData[1].split('`');
            if (2 != transStop.size())
            {
                continue;
            }

            qreal percent = transStop[0].toFloat();
            int value = transStop[1].toInt();

            QTransStop stop = QPair<qreal, int>(percent, value);
            transStops.append(stop);
        }
    }

    colorData.setColorStops(colorStops);
    colorData.setTransStops(transStops);

    return colorData;
}

bool ColorData::saveToFile(const QString &fileName) const
{
    if (QFile::exists(fileName))
    {
        QFile::remove(fileName);
    }

    QFile file(fileName);
    if (!file.open(QFile::ReadWrite))
    {
        return false;
    }

    QTextStream textStream(&file);

    int i = 0;
    QString colorName(m_colorName);
    if (colorName.isEmpty())
        colorName = QFileInfo(fileName).baseName();
    QString value = QString("%1.Name:%2").arg(i).arg(colorName);
    textStream << value << "\n";

    int j = 0;
    foreach (const QGradientStop &gradient, m_colorStops)
    {
        value = QString("%1.Value-Color.%2:%3`%4`%5`%6`%7").arg(i).arg(j++)
                .arg(gradient.first)
                .arg(gradient.second.red())
                .arg(gradient.second.green())
                .arg(gradient.second.blue())
                .arg(gradient.second.alpha());

        textStream << value << "\n";
    }

    int k = 0;
    foreach (const QTransStop &trans, m_transStops)
    {
        value = QString("%1.Transparency.%2:%3`%4").arg(i).arg(k++)
                .arg(trans.first)
                .arg(trans.second);

        textStream << value << "\n";
    }

    textStream << "\n\n";

    return true;
}

ColorData::ColorData(const QString &colorName)
    : m_colorName(colorName)
{
}

bool ColorData::isValid() const
{
    return m_colorName.isEmpty();
}

QString ColorData::getColorName() const
{
    return m_colorName;
}

void ColorData::setColorName(const QString &colorName)
{
    m_colorName = colorName;
}

bool ColorData::operator==(const ColorData& rhs) const
{
    if(m_colorName!=rhs.m_colorName) return false;
    if(m_colorStops!=rhs.m_colorStops) return false;
    if(m_transStops!=rhs.m_transStops) return false;

    return true;
}

bool ColorData::operator!=(const ColorData& rhs) const
{
    return !(rhs == *this);
}

void ColorData::setColorStops(const QGradientStops &stops)
{
    m_colorStops = stops;
}

void ColorData::updateColorOfPoint(int index, const QColor &color)
{
    if (index>=0 && index<m_colorStops.size())
    {
        m_colorStops[index].second = color;
    }
}

void ColorData::setTransStops(const QTransStops &stops)
{
    m_transStops = stops;
}

static void ratio_reverse(QGradientStop &stop)
{
    stop.first = 1.0 - stop.first;
}

void ColorData::reverse()
{
    std::for_each(m_colorStops.begin(), m_colorStops.end(), ratio_reverse);
    std::reverse(m_colorStops.begin(), m_colorStops.end());
}

QImage ColorData::toImage(int w, int h) const
{
    QLinearGradient lineGradient = QLinearGradient(QPointF(0, 0), QPointF(w, 0));
    for (int i=0; i<m_colorStops.size(); ++i)
    {
        lineGradient.setColorAt(m_colorStops.at(i).first, m_colorStops.at(i).second);
    }

    QImage colorImage(w, h, QImage::Format_RGB32);
    QPainter p(&colorImage);
    p.fillRect(colorImage.rect(), lineGradient);

    return colorImage;
}

QwtLinearColorMap *ColorData::toLinearColorMap() const
{
    if (m_colorStops.isEmpty())
        return 0;

    QwtLinearColorMap *linearColorMap = new QwtLinearColorMap(m_colorStops.first().second, m_colorStops.last().second);
    for(int i=1; i<m_colorStops.size()-1; ++i)
    {
        linearColorMap->addColorStop(m_colorStops.at(i).first, m_colorStops.at(i).second);
    }

    return linearColorMap;
}

QVector<QPointF> ColorData::colorStops(double minValue, double maxValue) const
{
    QVector<QPointF> ret;
    foreach (const QGradientStop &stop, m_colorStops)
    {
        double x = minValue + (maxValue - minValue) * stop.first;
        double y = 5; // [0, 10]

        ret.append(QPointF(x, y));
    }

    return ret;
}

QVector<QRgb> ColorData::generateColorValues_igeoseis() const
{
    QImage colorImage(toImage());
    QVector<QRgb> colorValues;
    for (int i=0; i<256; ++i)
    {
        float x = (float(i)/256.0)*colorImage.width();
        QRgb value = colorImage.pixel(QPoint(qMin(qRound(x), colorImage.width()), 0));
        colorValues.append(value);
    }

    return colorValues;
}

QVector<QRgb> ColorData::generateColorValues_qwt() const
{    
    if (m_colorStops.size()>=2)
    {
        QwtLinearColorMap linearColorMap(m_colorStops.first().second, m_colorStops.last().second);
        for(int i=1; i<m_colorStops.size()-1; ++i)
        {
            linearColorMap.addColorStop(m_colorStops.at(i).first, m_colorStops.at(i).second);
        }

        return linearColorMap.colorTable(QwtInterval(0, 255));
    }

    Q_ASSERT ("never come to here!");
    qWarning() << Q_FUNC_INFO << QString("error! never come to here!");
    return QVector<QRgb>();
}

QColor ColorData::color(double minValue, double maxValue, double value) const
{
    if (m_colorStops.size()>=2)
    {
        QwtLinearColorMap linearColorMap(m_colorStops.first().second, m_colorStops.last().second);
        for(int i=1; i<m_colorStops.size()-1; ++i)
        {
            linearColorMap.addColorStop(m_colorStops.at(i).first, m_colorStops.at(i).second);
        }

        return linearColorMap.color(QwtInterval(minValue, maxValue), value);
    }

    Q_ASSERT ("never come to here!");
    qWarning() << Q_FUNC_INFO << QString("error! never come to here!");
    return QColor();
}

QVector<QPointF> ColorData::transStops(double minValue, double maxValue) const
{
    QVector<QPointF> ret;
    foreach (const QTransStop &stop, m_transStops)
    {
        double x = minValue + (maxValue - minValue) * stop.first;
        double y = stop.second; // [0, 255]

        ret.append(QPointF(x, y));
    }

    return ret;
}

QVector<quint8> ColorData::generateTransValues() const
{
    // TODO： 对透明度插值，能否找一个现成的算法？——最好是某个第三方库的调用
    QVector<QPointF> points;
    foreach(QTransStop stop, m_transStops)
    {
        qreal x = stop.first*256;
        qreal y = stop.second;
        points.append(QPointF(x, y));
    }

    QVector<quint8> transValues;
    for (int x=0; x<256; ++x)
    {
        for (int i=1; i<points.size(); ++i)
        {
            if ( (points.at(i-1).x()<=x) && (points.at(i).x()>=x) )
            {
                QLineF lineF(points.at(i-1), points.at(i));
                lineF.setLength(lineF.length() * ((x - lineF.x1()) / lineF.dx()));
                transValues.append(qRound(qMin(lineF.y2(), qreal(255))));
            }
        }
    }

    return transValues;
}
