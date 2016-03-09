#ifndef SEIS3DSURVEY_H
#define SEIS3DSURVEY_H

#include <QString>
#include <QCache>

#include "seishorizon.h"

class Seis3DSurvey
{
public:
    explicit Seis3DSurvey(const QString &surveyName = QString());

    int startLine() const;
    void setStartLine(int startLine);

    int endLine() const;
    void setEndLine(int endLine);

    int dLine() const;
    void setDLine(int dLine);

    int startCdp() const;
    void setStartCdp(int startCdp);

    int endCdp() const;
    void setEndCdp(int endCdp);

    int dCdp() const;
    void setDCdp(int dCdp);

    int lineNumber() const;

    int cdpNumber() const;

    int pointNumber() const;

    int traceIndex(int line, int cdp) const;

    SurveyMatrixData getSeisHorizonData(const QString &seisHorizonName) const;

private:
    QString mSurveyName;
    int m_startLine, m_endLine, m_dLine, m_startCdp, m_endCdp, m_dCdp;

    mutable QCache<QString, SeisHorizon> mCache;
};

#endif // SEIS3DSURVEY_H
