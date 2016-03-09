#include "surveymatrixdata.h"

#include <functional>

#include <QDebug>
#include <QVector>

//#include "qigglobal.h"
#include "seis3dsurvey.h"

SurveyMatrixData::SurveyMatrixData()
{

}

SurveyMatrixData::SurveyMatrixData(Seis3DSurvey *survey) :
    mRange(survey)
{
//    for(int i=0; i<mRange->pointNumber(); ++i)
//        mDatas.append(QIG::nullValue<float>());
}

void SurveyMatrixData::setRange(Seis3DSurvey *survey)
{
    mRange = survey;
}

const float &SurveyMatrixData::getValue(int line, int cdp) const
{
    int traceIdx = mRange->traceIndex(line, cdp);
    Q_ASSERT(traceIdx>=0 && traceIdx<mDatas.size());
    if (traceIdx<0 || traceIdx>=mDatas.size())
    {
        qWarning()<<"error! out-of-range. traceIndex="<<traceIdx<<" m_datas.size()="<<mDatas.size();
//        static const float nullvalue = QIG::nullValue<float>();
//        return nullvalue;
    }

    return mDatas[traceIdx];
}

void SurveyMatrixData::setValue(int line, int cdp, float value)
{
    int traceIdx = mRange->traceIndex(line, cdp);
    Q_ASSERT(traceIdx>=0 && traceIdx<mDatas.size());
    if (traceIdx<0 || traceIdx>=mDatas.size())
    {
        qWarning()<<"error! out-of-range. traceIndex="<<traceIdx<<" m_datas.size()="<<mDatas.size();
        return;
    }

    mDatas[traceIdx] = value;
}

QList<float> SurveyMatrixData::getLine(int line) const
{
    // TODO: 此方法有一种更高效率的实现
    // 为了与getCdp()保持一致，这里暂时未采用
    QList<float> cdpValues;
    for (int iCdp=mRange->startCdp(); iCdp<=mRange->endCdp(); iCdp+=mRange->dCdp())
    {
        cdpValues.append(getValue(line, iCdp));
    }

    return cdpValues;
}

QList<float> SurveyMatrixData::getCdp(int cdp) const
{
    QList<float> lineValues;
    for (int iLine=mRange->startLine(); iLine<=mRange->endLine(); iLine+=mRange->dLine())
    {
        lineValues.append(getValue(iLine, cdp));
    }

    return lineValues;
}

QList<float> SurveyMatrixData::getPoints(const QList<SurveyPoint> &lineCdps) const
{
    QList<float> values;
    foreach (const SurveyPoint &var, lineCdps)
    {
        values.append(getValue(var.line, var.cdp));
    }

    return values;
}
