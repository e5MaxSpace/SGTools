#include "seis3dsurvey.h"


Seis3DSurvey::Seis3DSurvey(const QString &surveyName) :
    mSurveyName(surveyName),
    m_startLine(1900), m_endLine(2040), m_dLine(1),
    m_startCdp(1500), m_endCdp(1790), m_dCdp(1)
{
}

int Seis3DSurvey::startLine() const
{
    return m_startLine;
}

void Seis3DSurvey::setStartLine(int startLine)
{
    m_startLine = startLine;
}

int Seis3DSurvey::endLine() const
{
    return m_endLine;
}

void Seis3DSurvey::setEndLine(int endLine)
{
    m_endLine = endLine;
}

int Seis3DSurvey::dLine() const
{
    return m_dLine;
}

void Seis3DSurvey::setDLine(int dLine)
{
    m_dLine = dLine;
}

int Seis3DSurvey::startCdp() const
{
    return m_startCdp;
}

void Seis3DSurvey::setStartCdp(int startCdp)
{
    m_startCdp = startCdp;
}

int Seis3DSurvey::endCdp() const
{
    return m_endCdp;
}

void Seis3DSurvey::setEndCdp(int endCdp)
{
    m_endCdp = endCdp;
}

int Seis3DSurvey::dCdp() const
{
    return m_dCdp;
}

void Seis3DSurvey::setDCdp(int dCdp)
{
    m_dCdp = dCdp;
}

int Seis3DSurvey::lineNumber() const
{
    return (m_endLine-m_startLine)/m_dLine+1;
}

int Seis3DSurvey::cdpNumber() const
{
    return (m_endCdp-m_startCdp)/m_dCdp+1;
}

int Seis3DSurvey::pointNumber() const
{
    return lineNumber()*cdpNumber();
}

int Seis3DSurvey::traceIndex(int line, int cdp) const
{
    return ((line-m_startLine)/m_dLine)*cdpNumber() + (cdp-m_startCdp)/m_dCdp;
}

SurveyMatrixData Seis3DSurvey::getSeisHorizonData(const QString &seisHorizonName) const
{
    if (!mCache.contains(seisHorizonName))
    {
        mCache.insert(seisHorizonName, new SeisHorizon(seisHorizonName));
    }

    return mCache[seisHorizonName]->getSeisHorizonData();
}
