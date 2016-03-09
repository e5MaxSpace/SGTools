#include "seistest.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>

#include "seis3dsurvey.h"

SeisTest::SeisTest()
{
}

SurveyMatrixData SeisTest::getData(QString mfileName)
{
    QString fileName(mfileName/*"F:/SeiSDATAS/BB2428/P1.txt"*/);
    if (fileName.isEmpty())
    {
        return SurveyMatrixData();
    }
    QFile file(fileName);
    if ( !file.open(QIODevice::ReadOnly) )
    {
        qWarning()<<Q_FUNC_INFO<<"open file failed."<<file.errorString();
        return SurveyMatrixData();
    }

    Seis3DSurvey *mRange = new Seis3DSurvey();
    SurveyMatrixData data(mRange);
    QTextStream textStream(&file);
    int rowCount = 0;
    int startRow = 2;
    while (!textStream.atEnd())
    {
        QStringList contents = textStream.readLine().trimmed().simplified().split(" ");
        ++rowCount;
        if (rowCount<startRow)
        {
            continue;
        }

        int line = -1;
        int cdp  = -1;
        /*
        if (m_isCoord)
        {
            float x = m_recordParser.value(DbHorizonData::XCoord, content).toDouble();
            float y = m_recordParser.value(DbHorizonData::YCoord, content).toDouble();
            m_survey.xyToLineCdp(x, y, line, cdp);
        }
        else
        */
        {
            line = contents.at(2).toInt();
            cdp  = contents.at(3).toInt();
        }

        /*
        if (!m_survey.isInSurvey(line, cdp))
        {
            continue;
        }*/

        float time = contents.at(4).toFloat();

        data.setValue(line, cdp, time);
    }
    file.close();
    delete mRange;

    return data;
}
