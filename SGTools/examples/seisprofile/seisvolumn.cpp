#include "seisvolumn.h"

#include <QFile>
#include <QDebug>

#include "segyread.h"

SeisVolumn::SeisVolumn(QString fileName)
{
    QString segyFileName(fileName/*"F:/SeiSDATAS/BB2428/seisfile.segy"*/);
    m_file = new QFile(segyFileName);
    if (!m_file->open(QFile::ReadOnly))
    {
        qWarning()<<Q_FUNC_INFO<<QString("打开地震数据文件失败！文件：")<<segyFileName;
    }

    const int fileHeaderPrefixLength=3200;
    const int fileHeaderSuffixLength=400;
    m_isBigEndian = true;

    m_file->seek(fileHeaderPrefixLength);
    char buffer[fileHeaderSuffixLength];
    m_file->read(buffer, fileHeaderSuffixLength);

    /** 采样格式、采样间隔和采样数量 */
    sampleFormat = SegyRead::mid(buffer+25-1, 2, m_isBigEndian);
    sampleNumber = SegyRead::mid(buffer+21-1, 2, m_isBigEndian);
    sampleInterval = SegyRead::mid(buffer+17-1, 2, m_isBigEndian);
    qDebug()<<"sampleFormat="<<sampleFormat<<" sampleNumber="<<sampleNumber<<" sampleInterval="<<sampleInterval;
}

int SeisVolumn::traceIndex(int line, int cdp) const
{
    int traceIndex = ((line-1900)/1)*(291) + (cdp-1500)/1;
    return traceIndex;
}

QVector<float> SeisVolumn::getTrace(int line, int cdp) const
{
    int trace = traceIndex(line, cdp);
    qDebug()<<"trace="<<trace;

    const qint64 aTraceDataLength = qint64(sampleNumber)*qint64(SegyRead::sampleFormat2Bytes(sampleFormat));

    SegyRead segy;
    segy.setFormat(sampleFormat);
    segy.setSampleNumber(sampleNumber);
    segy.setBigEndian(m_isBigEndian);

    qint64 offset = fileHeaderLength + trace*(traceHeaderLength + aTraceDataLength) + traceHeaderLength;
    m_file->seek(offset);

    QVector<char> data(aTraceDataLength);
    m_file->read(data.data(), aTraceDataLength);

    QVector<float> values( sampleNumber );
    values.fill(0);
    segy.binaryConvert(values.data(), data.data(), sampleNumber);

    return values;
}

SeisProfileData SeisVolumn::getLine(int line) const
{
    SeisProfileData datas;
    datas.setTimes(0, sampleInterval, sampleNumber);
    for (int iCdp=1500; iCdp<=1790; iCdp+=1)
    {
        datas.appendOneTraceData(line, iCdp, getTrace(line, iCdp));
    }

    return datas;
}

SeisProfileData SeisVolumn::getCdp(int cdp) const
{
    SeisProfileData datas;
    datas.setTimes(0, sampleInterval, sampleNumber);
    for (int iLine=1900; iLine<=2040; iLine+=1)
    {
        datas.appendOneTraceData(iLine, cdp, getTrace(iLine, cdp));
    }

    return datas;
}

SeisProfileData SeisVolumn::getPoints(const QList<SurveyPoint> &lineCdps) const
{
    SeisProfileData datas;
    //for ()
    {
        //datas.appendOneTraceData(iLine, cdp, getTrace(iLine, cdp));
    }

    return datas;
}
