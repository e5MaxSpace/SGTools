#ifndef SEISHORIZON_H
#define SEISHORIZON_H

#include <QObject>
#include <QCache>

#include "surveymatrixdata.h"

class SeisHorizon : public QObject
{
    Q_OBJECT

public:
    explicit SeisHorizon(const QString &horizonName, QObject *parent = 0);

    const SurveyMatrixData &getSeisHorizonData() const;

    SurveyMatrixData *getSeisHorizonAttriData(const QString &attriName) const;

private:
    QString mHorizonName;
    SurveyMatrixData mTimeAttri;
    mutable QCache<QString, SurveyMatrixData> mOtherAttriCache;
};

#endif // SEISHORIZON_H
