#include "seisprofiledata.h"

#include <QDebug>

SeisProfileData::SeisProfileData() :
    m_startTime_ms(0.0f), m_dt_ms(0.0f), m_ns(0)
{

}

SeisProfileData::SeisProfileData(float startTime_ms, float dt_ms, int ns) :
    m_startTime_ms(startTime_ms), m_dt_ms(dt_ms), m_ns(ns)
{

}

void SeisProfileData::setTimes(float startTime_ms, float dt_ms, int ns)
{
    m_startTime_ms = startTime_ms;
    m_dt_ms = dt_ms;
    m_ns = ns;
}

void SeisProfileData::appendOneTraceData(int line, int cdp, const QVector<float> &traceData)
{
    Q_ASSERT_X(traceData.size() == m_ns, "SeisProfileData::appendOneTraceData", "error! traceData.size() != ns");
    if (traceData.size() != m_ns)
    {
        qWarning()<<"error! traceData.size() != ns"<<" traceData.size()="<<traceData.size()<<" ns="<<m_ns;
        return;
    }

    SeisTraceData aSeisTraceData;
    aSeisTraceData.line = line;
    aSeisTraceData.cdp  = cdp;
    aSeisTraceData.traceData = traceData;

    m_traceDatas.append(aSeisTraceData);
}

int SeisProfileData::trace(int line, int cdp) const
{
    for (int i=0; i<m_traceDatas.size(); ++i)
    {
        if ((m_traceDatas[i].line == line) && (m_traceDatas[i].cdp == cdp))
            return i;
    }

    Q_ASSERT(!"never come to here!");
    qWarning()<<"error! never come to here! line="<<line<<" cdp="<<cdp;

    return -1;
}

QList<QVector<float> > SeisProfileData::profileData() const
{
    QList<QVector<float> > profileData;
    foreach (const SeisTraceData &aSeisTraceData, m_traceDatas)
    {
        profileData.append(aSeisTraceData.traceData);
    }

    return profileData;
}
