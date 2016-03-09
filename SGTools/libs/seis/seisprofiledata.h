#ifndef SEISPROFILEDATA_H
#define SEISPROFILEDATA_H

#include <QVector>

class SeisProfileData
{
public:
    SeisProfileData();
    SeisProfileData(float startTime_ms, float dt_ms, int ns);

    void setTimes(float startTime_ms, float dt_ms, int ns);
    void clear() { m_traceDatas.clear(); }
    void appendOneTraceData(int line, int cdp, const QVector<float> &traceData);
    int traceCount() const { return m_traceDatas.size(); }
    int line(int trace) const { return m_traceDatas[trace].line; }
    int cdp(int trace) const { return m_traceDatas[trace].cdp; }
    int trace(int line, int cdp) const;
    const QVector<float> &traceData(int trace) const { return m_traceDatas[trace].traceData; }
    QList<QVector<float> > profileData() const;

private:
    float m_startTime_ms;
    float m_dt_ms;
    int m_ns;

    struct SeisTraceData
    {
        int line;
        int cdp;
        QVector<float> traceData;
    };
    QList<SeisTraceData> m_traceDatas;
};

#endif // SEISPROFILEDATA_H
