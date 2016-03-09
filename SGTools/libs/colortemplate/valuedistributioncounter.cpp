#include "valuedistributioncounter.h"

#include <QTime>

#include <algorithm>
#include <limits>

class IsInRange
{
public:
    IsInRange(float st, float et, float ignoredValue = -9999.0) :
        m_st(st), m_et(et), m_ignoredValue(ignoredValue) {}

    bool operator() (float v) const
    {
        if (qAbs(v - m_ignoredValue)<=1e-6)
            return false;

        return (v>=m_st && v<m_et);
    }

private:
    const float m_st, m_et;
    const float m_ignoredValue;
};

ValueDistributionCounter::ValueDistributionCounter(int steps) : m_steps(steps), m_ignoredValue(-9999.0)
{
    if (m_steps<10)
        m_steps = 10;
}

void ValueDistributionCounter::setValues(const QVector<float> &values)
{
    m_values = values;
}

void ValueDistributionCounter::setMinValue(float minValue)
{
    m_minValue = minValue;
}

void ValueDistributionCounter::setMaxValue(float maxValue)
{
    m_maxValue = maxValue;
}

void ValueDistributionCounter::setIgnoredValue(float ignoredValue)
{
    m_ignoredValue = ignoredValue;
}

QVector<QwtIntervalSample> ValueDistributionCounter::valueDistribution() const
{
    QTime time;
    time.start();

    if (m_values.isEmpty())
        return QVector<QwtIntervalSample>();

    float delta = (m_maxValue - m_minValue)/m_steps;
    QVector<QwtIntervalSample> statisticResult;
    for (int i=0; i<m_steps; ++i)
    {
        float st = m_minValue + i*delta;
        float et = st + delta;
        float value = std::count_if(m_values.constBegin(), m_values.constEnd(), IsInRange(st, et));
        statisticResult.push_back(QwtIntervalSample(value, QwtInterval(st, et)));
    }

    qDebug()<<"valueDistribution-time="<<time.elapsed();

    return statisticResult;
}
