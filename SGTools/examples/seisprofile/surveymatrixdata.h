#ifndef SURVEYMATRIXDATA_H
#define SURVEYMATRIXDATA_H

#include <QList>

#include "surveypoint.h"

class Seis3DSurvey;

class SurveyMatrixData
{
public:
    SurveyMatrixData();

    explicit SurveyMatrixData(Seis3DSurvey *survey);

    void setRange(Seis3DSurvey *survey);

    const float &getValue(int line, int cdp) const;

    void setValue(int line, int cdp, float value);

    QList<float> getLine(int line) const;

    QList<float> getCdp(int cdp) const;

    QList<float> getPoints(const QList<SurveyPoint> &lineCdps) const;

private:
    Seis3DSurvey *mRange;
    QList<float> mDatas;
};

#endif // SURVEYMATRIXDATA_H
