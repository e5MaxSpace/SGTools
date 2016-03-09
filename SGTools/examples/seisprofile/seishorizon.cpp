#include "seishorizon.h"

#include "seistest.h"

SeisHorizon::SeisHorizon(const QString &horizonName, QObject *parent) :
    QObject(parent), mHorizonName(horizonName)
{
    mTimeAttri = SeisTest::getData(horizonName);
}

const SurveyMatrixData &SeisHorizon::getSeisHorizonData() const
{
    return mTimeAttri;
}

SurveyMatrixData *SeisHorizon::getSeisHorizonAttriData(const QString &attriName) const
{
    if (!mOtherAttriCache.contains(attriName))
    {
        //mOtherAttriCache.insert(attriName, getData(mfileName));
    }

    return mOtherAttriCache[attriName];
}
