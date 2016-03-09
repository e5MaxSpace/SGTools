#ifndef SEISVOLUMN_H
#define SEISVOLUMN_H

#include "seis3dsurvey.h"
#include "seisprofiledata.h"

class QFile;
class SeisVolumn
{
public:
    SeisVolumn(QString fileName);

    QVector<float> getTrace(int line, int cdp) const;

    SeisProfileData getLine(int line) const;

    SeisProfileData getCdp(int cdp) const;

    SeisProfileData getPoints(const QList<SurveyPoint> &lineCdps) const;

    int traceIndex(int line, int cdp) const;

private:
    QFile *m_file;

    bool m_isBigEndian;
    int sampleFormat;
    int sampleNumber;
    int sampleInterval;
};

#endif // SEISVOLUMN_H
