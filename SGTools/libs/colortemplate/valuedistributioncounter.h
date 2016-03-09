#ifndef VALUEDISTRIBUTIONCOUNTER_H
#define VALUEDISTRIBUTIONCOUNTER_H

#include <QVector>

#include <qwt_samples.h>

class ValueDistributionCounter
{
public:
    explicit ValueDistributionCounter(int steps = 50/*255*/);

    void setValues(const QVector<float> &values);
    void setMinValue(float minValue);
    void setMaxValue(float maxValue);
    void setIgnoredValue(float ignoredValue);

    QVector<QwtIntervalSample> valueDistribution() const;

private:
    int m_steps;
    float m_ignoredValue;

    QVector<float> m_values;

    float       m_minValue;
    float       m_maxValue;
};

#endif // VALUEDISTRIBUTIONCOUNTER_H
