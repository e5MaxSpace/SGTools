#ifndef SEISTEST_H
#define SEISTEST_H

#include "surveymatrixdata.h"

class SeisTest
{
public:
    SeisTest();

    static SurveyMatrixData getData(QString mfileName);
};

#endif // SEISTEST_H
