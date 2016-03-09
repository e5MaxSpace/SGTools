#include "seisprofileitem.h"

#include <limits>
#include <numeric>
#include <algorithm>
#include <functional>

#include <qmath.h>
#include <qnumeric.h>

#include <QPen>
#include <QBrush>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

class inSeisProfileData
{
public:
    inSeisProfileData() :
        m_profileMinValue(1),m_profileMaxValue(1),
        m_profileAbsMaxValue(1),m_traceMaxValueAverage(1)
    {
        init();
    }

    inSeisProfileData(const QList<QVector<float> > &traceData) :
        m_traceData(traceData),m_profileMinValue(1),m_profileMaxValue(1),
        m_profileAbsMaxValue(1),m_traceMaxValueAverage(1)
    {
        init();
    }

    bool isNull() const { return m_traceData.isEmpty();}

    int traceCount() const { return m_traceData.size();}

    int ns() const
    {
        if (m_traceData.isEmpty())
            return 0;

        return m_traceData.first().size();
    }

    float value(int traceIndex, int sampleIndex) const
    {
        return m_traceData[traceIndex][sampleIndex];
    }

    const QVector<float> &traceData(int traceIndex) const
    {
        Q_ASSERT(traceIndex>=0 && traceIndex<m_traceData.size());
        return m_traceData[traceIndex];
    }
    //初始化各个最大最小值
    void init()
    {
        QVector<float> vecMaxAmp, vecMinAmp , vecAbsMaxAmp;
        for(int i=0; i<m_traceData.size(); i++)
        {
            if(m_traceData[i].size())
            {
                float minAmp = (float)(*std::min_element(m_traceData[i].begin(), m_traceData[i].end()));
                vecMinAmp.push_back(minAmp);

                float maxAmp = (float)(*std::max_element(m_traceData[i].begin(), m_traceData[i].end()));
                vecMaxAmp.push_back(maxAmp);

                vecAbsMaxAmp.push_back(qMax(qAbs(minAmp), qAbs(maxAmp)));
            }
        }

        if(vecMinAmp.size())
            m_profileMinValue = (float)(*std::min_element(vecMinAmp.begin(), vecMinAmp.end()));

        if(vecMaxAmp.size())
            m_profileMaxValue = (float)(*std::max_element(vecMaxAmp.begin(), vecMaxAmp.end()));

        m_profileAbsMaxValue = qMax(qAbs(m_profileMinValue), qAbs(m_profileMaxValue));

        double sum = 0.0;
        m_traceMaxValueAverage = std::accumulate(vecAbsMaxAmp.begin(),
                                                 vecAbsMaxAmp.end(),sum)/vecAbsMaxAmp.size();

    }
private:
    QList<QVector<float> > m_traceData;

    float m_profileMinValue;  //数据体最小值
    float m_profileMaxValue;  //数据体最大值
    float m_profileAbsMaxValue; //数据体绝对值最大值
    float m_traceMaxValueAverage;//各道数据绝对值的平均值
    friend class SeisProfileItem;
};

class DrawPathGenerator
{
public:
    explicit DrawPathGenerator(const QPointF &pos, const QVector<float> &aTraceData, int yscale)
    {
        for(int i=0; i<aTraceData.size(); ++i)
        {
            m_polygon.push_back(QPointF(pos.x()+aTraceData[i], pos.y()+i*yscale));
        }

        m_pos = pos;
    }

    const QPolygonF &getPolygon() const
    {
        return m_polygon;
    }
    QPainterPath &getLeftFillPath()
    {
        generatePainterPath(m_pos, m_polygon, m_fillPath, false);
        return m_fillPath;
    }
    QPainterPath &getRightFillPath()
    {
        generatePainterPath(m_pos, m_polygon, m_fillPath, true);
        return m_fillPath;
    }
    const QPainterPath &getFillPath()
    {
        return m_fillPath;
    }
private:
    QPolygonF m_polygon;
    QPainterPath m_fillPath;
    QPointF m_pos;

private:
    QPointF generateZeroPoint(const QPointF& pt0, const QPointF& pt1, const qreal baseValue)
    {
        qreal dx = qAbs((pt0.x() - baseValue)/(pt1.x() - baseValue));
        qreal y  = (pt0.y()+dx*pt1.y())/(dx + 1);

        return QPointF(baseValue, y);
    }
    QPolygonF removeNaN(const QPolygonF &aPolygon)
    {
        QPolygonF resPolygon;
        foreach (const QPointF &p, aPolygon)
        {
            if (!qIsNaN(p.x()) && !qIsNaN(p.y()))
            {
                resPolygon.append(p);
            }
        }

        return resPolygon;
    }

    void generatePainterPath( QPointF &pos,  QPolygonF &polygon, QPainterPath &fillPath, bool fillRight )
    {
        int size = polygon.size();
        if(!size)
            return;
        float flag = -1;
        if(!fillRight)
            flag=1;

        QPolygonF semiwavePolygon;
        bool firstflag = true;
        for(int i=0; i<size; ++i)
        {
            if((polygon[i].x()-pos.x())*flag < 0)
            {
                if(firstflag)
                {
                    if(i==0)//第一个点特殊处理
                        semiwavePolygon.push_back(QPointF(pos.x(), polygon[i].y()));
                    else
                        semiwavePolygon.push_back(generateZeroPoint(polygon[i-1], polygon[i], pos.x()));
                    firstflag = false;
                }

                semiwavePolygon.push_back(polygon[i]);

                if(i==size-1)//最后一个点特殊处理
                {
                    semiwavePolygon.push_back(QPointF(pos.x(), polygon[i].y()));
                    fillPath.addPolygon(removeNaN(semiwavePolygon));
                }
            }
            else
            {
                if(!firstflag)
                {
                    semiwavePolygon.push_back(generateZeroPoint(polygon[i-1], polygon[i], pos.x()));
                    fillPath.addPolygon(removeNaN(semiwavePolygon));
                    semiwavePolygon.clear();
                    firstflag = true;
                }
            }
        }

    }
};

class SeisProfileItem::SeisProfileItemPrivate
{
public:
    explicit SeisProfileItemPrivate() :
        m_displayMode(SeisProfileItem::VarDensity),
        m_fillDirection(SeisProfileItem::Right),
        m_normalizationMode(SeisProfileItem::Average),
        m_colorDistributeMode(SeisProfileItem::Symmetric),
        m_transformationMode(Qt::SmoothTransformation), m_ampFactor(2), m_brush(Qt::SolidPattern)
    {
    }

    explicit SeisProfileItemPrivate(const QList<QVector<float> > &profileData) :
        m_originData(profileData),
        m_displayMode(SeisProfileItem::VarDensity),
        m_fillDirection(SeisProfileItem::Right),
        m_normalizationMode(SeisProfileItem::Average),
        m_colorDistributeMode(SeisProfileItem::Symmetric),
        m_transformationMode(Qt::SmoothTransformation), m_ampFactor(2), m_brush(Qt::SolidPattern)
    {
    }

    inSeisProfileData m_originData;

    SeisProfileItem::DisplayMode m_displayMode;
    SeisProfileItem::TraceFillDirection m_fillDirection;
    SeisProfileItem::NormalizationMode m_normalizationMode;
    SeisProfileItem::ColorDistributeMode m_colorDistributeMode;
    Qt::TransformationMode m_transformationMode;

    int m_ampFactor;

    QPen m_pen;
    QBrush m_brush;

    QVector<QRgb> m_colorMap;
};

SeisProfileItem::SeisProfileItem(QGraphicsItem *parent)
    : QGraphicsItem(parent), m_d(new SeisProfileItemPrivate())
{
    this->setFlag(QGraphicsItem::ItemUsesExtendedStyleOption);
}

SeisProfileItem::SeisProfileItem(const QList<QVector<float> > &profileData, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_d(new SeisProfileItemPrivate(profileData))
{
    this->setFlag(QGraphicsItem::ItemUsesExtendedStyleOption);
}

SeisProfileItem::~SeisProfileItem()
{
    delete m_d;
    m_d = 0;
}

void SeisProfileItem::setProfileData(const QList<QVector<float> > &profileData)
{
    qDebug()<<"profileData.size="<<profileData.size();
    m_d->m_originData = profileData;
    update();
}

SeisProfileItem::DisplayMode SeisProfileItem::displayMode() const
{
    return m_d->m_displayMode;
}

void SeisProfileItem::setDisplayMode(DisplayMode mode)
{
    if (mode != m_d->m_displayMode)
    {
        m_d->m_displayMode = mode;
        update();
    }
}

SeisProfileItem::TraceFillDirection SeisProfileItem::fillDirection() const
{
    return m_d->m_fillDirection;
}
void SeisProfileItem::setTraceFillDirection(TraceFillDirection direction)
{
    if(direction != m_d->m_fillDirection)
    {
        m_d->m_fillDirection = direction;
        update();
    }
}

SeisProfileItem::NormalizationMode SeisProfileItem::normalizationMode() const
{
    return m_d->m_normalizationMode;
}

void SeisProfileItem::setNormalizationMode(SeisProfileItem::NormalizationMode mode)
{
    if (mode != m_d->m_normalizationMode)
    {
        m_d->m_normalizationMode = mode;
        update();
    }
}

SeisProfileItem::ColorDistributeMode SeisProfileItem::colorDistributeMode() const
{
    return m_d->m_colorDistributeMode;
}

void SeisProfileItem::setColorDistributeMode(ColorDistributeMode mode)
{
    if(m_d->m_colorDistributeMode != mode)
    {
        m_d->m_colorDistributeMode = mode;
        update();
    }
}

const QVector<QRgb> SeisProfileItem::colorMap()
{
    return m_d->m_colorMap;
}

void SeisProfileItem::setColorMap(const QVector<QRgb> &vecColor)
{
    if (m_d->m_colorMap != vecColor)
    {
        m_d->m_colorMap = vecColor;
        update();
    }
}

Qt::TransformationMode SeisProfileItem::transformationMode() const
{
    return m_d->m_transformationMode;
}

void SeisProfileItem::setTransformationMode(Qt::TransformationMode mode)
{
    if (mode != m_d->m_transformationMode)
    {
        m_d->m_transformationMode = mode;
        update();
    }
}

size_t SeisProfileItem::amplitueFactor()
{
    return m_d->m_ampFactor;
}

void SeisProfileItem::setAmplitueFactor(size_t factor)
{
    if(m_d->m_ampFactor != factor)
    {
        m_d->m_ampFactor = factor;
        update();
    }
}

const QPen &SeisProfileItem::pen() const
{
    return m_d->m_pen;
}

void SeisProfileItem::setPen(const QPen &pen)
{
    if (pen != m_d->m_pen)
    {
        m_d->m_pen = pen;
        update();
    }
}

const QBrush &SeisProfileItem::fillBrush() const
{
    return m_d->m_brush;
}

void SeisProfileItem::setFillBrush(const QBrush &brush)
{
    if (brush != m_d->m_brush)
    {
        m_d->m_brush = brush;
        update();
    }
}

double SeisProfileItem::getUsedMaxAmp() const
{
    if(m_d->m_normalizationMode == Profile)
    {
        return m_d->m_originData.m_profileAbsMaxValue;
    }
    else if(m_d->m_normalizationMode == Average)
    {
        return m_d->m_originData.m_traceMaxValueAverage;
    }
    return 0.00001;
}

void SeisProfileItem::getColorMinMaxAmp(double& minAmp, double& maxAmp) const
{
    if(m_d->m_colorDistributeMode == Homogeneous)
    {
        minAmp = m_d->m_originData.m_profileMinValue;
        maxAmp = m_d->m_originData.m_profileMaxValue;
    }
    else if(m_d->m_colorDistributeMode == Symmetric)
    {
        maxAmp = getUsedMaxAmp();
        minAmp = -maxAmp;
    }
}

QRectF SeisProfileItem::boundingRect() const
{
    return QRectF(0, 0, m_d->m_originData.traceCount()+1, m_d->m_originData.ns()-1);
}

void SeisProfileItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (m_d->m_originData.isNull())
        return;

    ////////////////////////////////
    QTransform tf = painter->worldTransform();
    int xfactor = qCeil(1/tf.m11());
    int yfactor = qCeil(1/tf.m22());

    QRectF exposedRect(option->exposedRect);

    int startTrace = qFloor(exposedRect.left()) - xfactor;
    int endTrace = qCeil(exposedRect.right()) + xfactor;
    int startTime = qFloor(exposedRect.top()) - yfactor;
    int endTime = qCeil(exposedRect.bottom()) + yfactor;

    qDebug()<<"xfactor:"<<xfactor<<"yfactor:"<<yfactor;
    ////////////////////////////////
    if (startTrace < 1)
        startTrace = 1;
    if (endTrace > m_d->m_originData.traceCount())
        endTrace = m_d->m_originData.traceCount();

    if (startTime < 0)
        startTime = 0;
    if (endTime > m_d->m_originData.ns()-1)
        endTime = m_d->m_originData.ns()-1;

    ///////////////////////////////////////////////////////////////////
    qDebug()<<"startTrace:"<<startTrace<<" endTrace:"<<endTrace<<"time:"<<startTime<<' '<<endTime;
    ////////////////////////////////////////////////////////////////////
    //画颜色图
    switch (m_d->m_displayMode)
    {
    case SeisProfileItem::VarDensity:
    case SeisProfileItem::VarDensityWave:
    case SeisProfileItem::VarDensityVarArea:
    case SeisProfileItem::VarDensityWaveVarArea:
        qDebug()<<"here here here here here:";
        drawImage(painter, m_d->m_originData, startTrace, endTrace, startTime, endTime);
        break;
    default:
        break;
    }
    ////////////////////////////////////////////////////////////////////
    //画波形图
    int mode = 0;
    switch (m_d->m_displayMode)
    {
    case SeisProfileItem::Wave:
        mode = mode|Curve;
        break;
    case SeisProfileItem::VarArea:
        mode =mode|Fill;
        break;
    case SeisProfileItem::WaveVarArea:
        mode =mode|Curve|Fill;
        break;
    case SeisProfileItem::VarDensityWave:
        mode =mode|Curve;
        break;
    case SeisProfileItem::VarDensityVarArea:
        mode =mode|Fill;
        break;
    case SeisProfileItem::VarDensityWaveVarArea:
        mode =mode|Curve|Fill;
        break;
    default:
        break;
    }
    if(mode)
    {
        m_d->m_ampFactor *= xfactor;
        drawTraces(painter, m_d->m_originData, startTrace, endTrace, xfactor,
                   startTime, endTime, yfactor, mode);
        m_d->m_ampFactor /= xfactor;
    }
    ////////////////////////////////////////////////////////////////////
}

void SeisProfileItem::drawImage(QPainter *painter, const inSeisProfileData &traceData,int begTrace,
                                int endTrace,int begTime, int endTime) const
{
    if(!m_d->m_colorMap.size())
    {
        qWarning()<<"SeisProfileItem::drawImage: no color table! ";
        return;
    }

    double minAmp,maxAmp;
    getColorMinMaxAmp(minAmp, maxAmp);

    //    QTime timer;
    //    timer.start();
    int width = (endTrace - begTrace)+1;
    int height = (endTime - begTime)+1;
    qDebug()<<"here here here here here: width="<<width<<" height="<<height;
    ////////////////////////////////////////////////////////////////////////////
    QImage image(width, height, QImage::Format_Indexed8);
    image.setColorTable(m_d->m_colorMap);

    int colorSize = m_d->m_colorMap.size();
    qDebug()<<"colorSize"<<colorSize<< "minAmp"<<minAmp<<"maxAmp"<<maxAmp;
    double range = maxAmp - minAmp;
    for (int i=0; i<image.width(); i++)
    {
        const QVector<float>& data = traceData.traceData(i+begTrace-1);
        for (int j=0; j<image.height(); j++)
        {
            double value = data[j+begTime];
            int index = 0;
            ///////////////////////////////////////////////////
            if(value <= minAmp)
                index = 0;
            else if(value >= maxAmp)
                index = colorSize-1;
            else
                index = (value - minAmp)*(colorSize-1)/range;

            //qDebug()<<"index="<<index;
            ///////////////////////////////////////////////////
            image.setPixel(i,j,index);
            ///////////////////////////////////////////////////
        }
    }
    //    qDebug()<<"time elasped1 : " <<timer.elapsed() ;
    painter->setRenderHint(QPainter::SmoothPixmapTransform,
                           (m_d->m_transformationMode == Qt::SmoothTransformation));

    QRectF destRect(begTrace, begTime, endTrace-begTrace, endTime-begTime);

    qDebug()<<"destRect="<<destRect;

    painter->drawImage(destRect, image);
    //    qDebug()<<"time elasped2 : " <<timer.elapsed() ;
}

void SeisProfileItem::drawTraces(QPainter *painter, const inSeisProfileData &traceData,
                                 int begTrace, int endTrace, int traceStep,
                                 int begTime, int endTime, int timeStep, int traceDrawMode) const

{
    painter->save();

    if (painter->pen() != m_d->m_pen)
        painter->setPen(m_d->m_pen);

    if (m_d->m_brush.style() != Qt::NoBrush && painter->brush() != m_d->m_brush)
        painter->setBrush(m_d->m_brush);

    painter->setRenderHint(QPainter::SmoothPixmapTransform,
                           (m_d->m_transformationMode == Qt::SmoothTransformation));

    for (int iTrace=begTrace; iTrace<=endTrace; iTrace += traceStep)
    {
        const QVector<float> &aTraceData = traceData.traceData(iTrace-1);
        if ( aTraceData.size() <= 2 )
            continue;
        //////////////////////////////////////////////////////////
        float maxAmp = getUsedMaxAmp()/m_d->m_ampFactor;
        QVector<float> traceData;
        for(int i=begTime; i<endTime; i+=timeStep)
        {
            traceData.push_back(aTraceData[i]/maxAmp);
        }
        //////////////////////////////////////////////////////////

        DrawPathGenerator pathGenerator(QPointF(iTrace, begTime), traceData, timeStep);

        if(traceDrawMode & Fill && m_d->m_fillDirection==Left)
            painter->fillPath(pathGenerator.getLeftFillPath(), m_d->m_brush);
        if(traceDrawMode & Fill && m_d->m_fillDirection==Right)
            painter->fillPath(pathGenerator.getRightFillPath(), m_d->m_brush);
        if(traceDrawMode & Curve)
            painter->drawPolyline(pathGenerator.getPolygon());
    }

    painter->restore();
}
