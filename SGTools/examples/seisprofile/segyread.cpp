#include "segyread.h"

#include <QFileInfo>
#include <QVector>
#include <QtEndian>
#include <QDebug>

#include <windows.h>
#include <iostream>

using namespace std;


#define __bswap_constant_16(x)   \
    ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))

#define __bswap_constant_32(x) \
    ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) |  \
    (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24))


void ibm_to_float(int from[], int to[], int n, int endian)
/***********************************************************************
ibm_to_float - convert between 32 bit IBM and IEEE floating numbers
************************************************************************
Input::
from		input vector
to		output vector, can be same as input vector
endian		byte order =0 little endian (DEC, PC's)
=1 other systems
*************************************************************************
Notes:
Up to 3 bits lost on IEEE -> IBM

Assumes sizeof(int) == 4

IBM -> IEEE may overflow or underflow, taken care of by
substituting large number or zero

Only integer shifting and masking are used.
*************************************************************************/
{
    register int fconv, fmant, i, t;

    for (i=0;i<n;++i) {
        fconv = from[i];
        if (endian==1) fconv = (fconv<<24) | ((fconv>>24)&0xff) |
                ((fconv&0xff00)<<8) | ((fconv&0xff0000)>>8);
        fmant = 0x00ffffff & fconv;
        if (!fmant)
            fconv=0;
        else {
            t = (int) ((0x7f000000 & fconv) >> 22) - 130;
            while (!(fmant & 0x00800000)) { --t; fmant <<= 1; }
            if (t > 254) fconv = (0x80000000 & fconv) | 0x7f7fffff;
            else if (t <= 0) fconv = 0;
            else fconv = (0x80000000 & fconv) |(t << 23)|(0x007fffff & fmant);
        }
        to[i] = fconv;
    }
    return;
}

bool SegyRead::read(const QString &fileName, int offset, int length, char *buffer)
{
    QFile segyfile(fileName);
    if (!segyfile.open(QIODevice::ReadOnly))
    {
        qWarning()<<Q_FUNC_INFO<<"Open file failed. fileName="<<fileName;
        return false;
    }

    segyfile.seek(offset);
    segyfile.read(buffer, length);
    return true;
}

int SegyRead::mid(char *buffer, int len, bool isBigEndian)
{
    /*unsigned*/ int value = 0;
    if (isBigEndian)
    {
        if (len == 4)
            value = qFromBigEndian<qint32>((const uchar*)buffer);
        if (len == 2)
            value = qFromBigEndian<qint16>((const uchar*)buffer);
    }
    else
    {
        if (len == 4)
        {
            value = *(reinterpret_cast<int*>(buffer));
        }
        else  if (len == 2)
        {
            value = *(reinterpret_cast<short*>(buffer));
        }
    }

    return value;
}

void SegyRead::set(char *p, int len, bool isBigEndian, int value)
{
    if (isBigEndian)
    {
        if (len == 2)
        {
            DWORD data = __bswap_constant_16((short)value);
            memcpy(p, &data, len);
        }
        else if (len == 4)
        {
            DWORD data = __bswap_constant_32(value);
            memcpy(p, &data, len);
        }
    }
    else
    {
        memcpy(p, &value, len);
    }
}

int SegyRead::sampleFormat2Bytes(int format)
{
    /** 采样格式到字节数的映射 */
    static int sampleFormat2Bytes[] = {0, 4, 4, 2, 0, 4, 0, 0, 1};

    if ((unsigned int)format >= sizeof(sampleFormat2Bytes))
    {
        return 0;
    }

    return sampleFormat2Bytes[format];
}

SegyRead::SegyRead(QObject *parent) :
    QObject(parent)
{
    m_currentLine = 0;
    m_currentLinePos = 0;
}

int SegyRead::open(const QString &fileName)
{
    m_currentLine = 0;
    m_currentLinePos = 0;
    m_traceLength = sampleFormat2Bytes(m_format) * m_sampleNumber;

    m_filestream.close();
    m_filestream.setFileName(fileName);
    if(!m_filestream.open(QFile::ReadOnly))
    {
        return -1;
    }

    if (m_currentLine == 0)
    {
        QFileInfo info(fileName);
        qint64 size = info.size();
        if (size <= 0)
        {
            return -1;
        }

        qint64 pos = fileHeaderPrefixLength+fileHeaderSuffixLength; ///< 文件头长度

        // 取出前2个trace的线号，用于判断递增方向
        int lineId1, lineId2;
        int cdpId1, cdpId2;

        char *buffer = new char[traceHeaderLength];
        m_filestream.seek(pos);
        m_filestream.read(buffer, traceHeaderLength);
        lineId1 = mid(buffer+m_lineOffset, m_lineLength, m_isBigEndian);
        cdpId1  = mid(buffer+m_cdpOffset, m_cdpLength, m_isBigEndian);
        pos += traceHeaderLength + m_traceLength;//道长 + 道头长度

        m_filestream.seek(pos);
        m_filestream.read(buffer, traceHeaderLength);
        lineId2 = mid(buffer+m_lineOffset, m_lineLength, m_isBigEndian);
        cdpId2  = mid(buffer+m_cdpOffset, m_cdpLength, m_isBigEndian);
        delete[] buffer;

        if (lineId1 == lineId2)
        {
            m_currentLine = lineId1;
            m_incrementDirection = InlineIncrement;
        }
        else if (cdpId1 == cdpId2)
        {
            m_currentLine = cdpId1;
            m_incrementDirection = XlineIncrement;
        }
        else
        {
            qWarning()<<"error.该文件的线道号值不正确！";
        }
        qDebug()<<"m_incrementDirection="<<m_incrementDirection;


        m_currentLinePos = fileHeaderPrefixLength+fileHeaderSuffixLength;
    }

    return 0;
}

int SegyRead::readLine(int beginLine, int endLine,
                       int beginCdp, int endCdp, int cdpIncrement,
                       int beginTime,int endTime,int timeIncrement,
                       int &currentLine, QVector<float> &data, int &percent)
{
    qint64 fileSize = m_filestream.size();
    int readLength = (endTime - beginTime)/timeIncrement + 1;
    int bPos = (beginTime-m_beginTime_us)/timeIncrement * sampleFormat2Bytes(m_format);

    QVector<char> buffer(readLength * sampleFormat2Bytes(m_format));

    if (m_currentLine < beginLine || m_currentLine > endLine)
    {
        // 不在范围内，需要继续读取一个新的line，直到line在范围或文件结束
        while (1)
        {
            if (m_currentLinePos >= fileSize)
            {
                return -1;
            }

            m_filestream.seek(m_currentLinePos);
            m_filestream.read(buffer.data(), traceHeaderLength);

            m_currentLine = mid(buffer.data()+m_lineOffset, m_lineLength, m_isBigEndian);
            if (m_currentLine >= beginLine && m_currentLine <= endLine)
            {
                break;
            }

            m_currentLinePos += (traceHeaderLength + m_traceLength);
        }
    }

    currentLine = m_currentLine;

    for (qint64 filePos = m_currentLinePos;
         filePos < fileSize;
         filePos += (traceHeaderLength + m_traceLength))
    {
        m_filestream.seek(filePos);
        m_filestream.read(buffer.data(), traceHeaderLength);

        int line = mid(buffer.data()+m_lineOffset, m_lineLength, m_isBigEndian);
        int cdp  = mid(buffer.data()+m_cdpOffset,  m_cdpLength,  m_isBigEndian);

        if (line != m_currentLine)
        {
            m_currentLine = line;
            m_currentLinePos = filePos;
            percent = m_currentLinePos*100.0/fileSize;
            return 0;
        }

        if (cdp>=beginCdp && cdp<=endCdp)
        {
            m_filestream.seek(filePos+traceHeaderLength+bPos);
            m_filestream.read(buffer.data(), readLength*sampleFormat2Bytes(m_format));

            int outDataPos = (cdp - beginCdp)/cdpIncrement*readLength;
            binaryConvert(&data[outDataPos], buffer.data(), readLength);
        }
    }

    percent = 100;
    return 0;
}

int SegyRead::readCdp(int beginCdp, int endCdp, int beginLine, int endLine, int lineIncrement,int beginTime,int endTime,int timeIncrement, int &currentLine, QVector<float> &data, int &percent)
{
    qint64 fileSize = m_filestream.size();
    int readLength = (endTime - beginTime)/timeIncrement + 1;
    int bPos = (beginTime-m_beginTime_us)/timeIncrement * sampleFormat2Bytes(m_format);

    QVector<char> buffer(readLength * sampleFormat2Bytes(m_format));

    if (m_currentLine < beginCdp || m_currentLine > endCdp)
    {
        // 不在范围内，需要继续读取一个新的line，直到line在范围或文件结束
        while (1)
        {
            if (m_currentLinePos >= fileSize)
            {
                return -1;
            }

            m_filestream.seek(m_currentLinePos);
            m_filestream.read(buffer.data(), traceHeaderLength);

            m_currentLine = mid(buffer.data()+m_cdpOffset, m_cdpLength, m_isBigEndian);
            if (m_currentLine >= beginLine && m_currentLine <= endLine)
            {
                break;
            }

            m_currentLinePos += (traceHeaderLength + m_traceLength);
        }
    }

    currentLine = m_currentLine;

    for (qint64 filePos = m_currentLinePos;
         filePos < fileSize;
         filePos += (traceHeaderLength + m_traceLength))
    {
        m_filestream.seek(filePos);
        m_filestream.read(buffer.data(), traceHeaderLength);

        int line = mid(buffer.data()+m_lineOffset, m_lineLength, m_isBigEndian);
        int cdp  = mid(buffer.data()+m_cdpOffset,  m_cdpLength,  m_isBigEndian);

        if (cdp != m_currentLine)
        {
            m_currentLine = cdp;
            m_currentLinePos = filePos;
            percent = m_currentLinePos*100/fileSize;
            return 0;
        }

        if (line >=beginLine && line <=endLine)
        {
            m_filestream.seek(filePos+traceHeaderLength+bPos);
            m_filestream.read(buffer.data(), readLength*sampleFormat2Bytes(m_format));

            int outDataPos = (line - beginLine)/lineIncrement*readLength;
            binaryConvert(&data[outDataPos], buffer.data(), readLength);
        }
    }

    percent = 100;
    return 0;
}

void SegyRead::readTraceValue(qint64 offset, int length, int &value)
{
    //char buffer[length];
    char *buffer = new char[length];
    m_filestream.seek(offset);
    m_filestream.read(buffer, length);

    value = mid(buffer, length, m_isBigEndian);
    delete buffer;
}

void SegyRead::get3Points(int xOffset, int xLength, int yOffset, int yLength,
                          SegyPoint &p1, SegyPoint &p2, SegyPoint &p3)
{
    qint64 totalSize = m_filestream.size();
    qint64 pos = fileHeaderPrefixLength+fileHeaderSuffixLength; ///< 文件头长度

    int line, cdp, x, y;

    // 先取第一个点
    while (pos < totalSize)
    {
        line = 0, cdp = 0, x = 0, y = 0;
        readTraceValue(pos + m_lineOffset, m_lineLength, line);
        readTraceValue(pos + m_cdpOffset, m_cdpLength, cdp);
        readTraceValue(pos + xOffset, xLength, x);
        readTraceValue(pos + yOffset, yLength, y);

        qDebug()<<"p1.line= "<<line<<", p1.cdp="<<cdp<<", p1.x="<<x<<", p1.y="<<y;
        if (line!=0 && cdp!=0 && x!=0 && y!=0)
        {
            p1.line = line;
            p1.cdp = cdp;
            p1.x = x;
            p1.y = y;
            break;
        }

        pos += traceHeaderLength + m_traceLength; //道长 + 道头长度
    }

    // 取相同线的最后一个点
    int currentLine = line;
    qDebug()<<"currentLine="<<currentLine;
    while (pos < totalSize)
    {
        line = 0, cdp = 0, x = 0, y = 0;
        readTraceValue(pos + m_lineOffset, m_lineLength, line);
        if (currentLine != line)
        {
            // 找到下一条线了，退回到最后一道的位置，取值
            pos -= traceHeaderLength + m_traceLength;//道长 + 道头长度
            readTraceValue(pos + m_lineOffset, m_lineLength, line);
            readTraceValue(pos + m_cdpOffset, m_cdpLength, cdp);
            readTraceValue(pos + xOffset, xLength, x);
            readTraceValue(pos + yOffset, yLength, y);

            qDebug()<<"p2.line= "<<line<<", p2.cdp="<<cdp<<", p2.x="<<x<<", p2.y="<<y;
            if (line!=0 && cdp!=0 && x!=0 && y!=0)
            {
                p2.line = line;
                p2.cdp = cdp;
                p2.x = x;
                p2.y = y;
                break;
            }
        }

        pos += traceHeaderLength + m_traceLength;//道长 + 道头长度
    }

    // 从文件最后一道开始，取一个有效点
    int numTrace = (totalSize - fileHeaderPrefixLength - fileHeaderSuffixLength)/(traceHeaderLength + m_traceLength);
    pos = (fileHeaderPrefixLength + fileHeaderSuffixLength) + (qint64)(traceHeaderLength + m_traceLength) * (numTrace-1);
    while (pos > 0)
    {
        line = 0, cdp = 0, x = 0, y = 0;
        readTraceValue(pos + m_lineOffset, m_lineLength, line);
        readTraceValue(pos + m_cdpOffset, m_cdpLength, cdp);
        readTraceValue(pos + xOffset, xLength, x);
        readTraceValue(pos + yOffset, yLength, y);

        qDebug()<<"p3.line= "<<line<<", p3.cdp="<<cdp<<", p3.x="<<x<<", p3.y="<<y;
        if (line!=0 && cdp!=0 && x!=0 && y!=0)
        {
            p3.line = line;
            p3.cdp = cdp;
            p3.x = x;
            p3.y = y;
            break;
        }

        pos -= traceHeaderLength + m_traceLength;//道长 + 道头长度
    }
}

void SegyRead::binaryConvert(void *destination, void *source, int length)
{
    if (!m_isBigEndian)
    {
        if (m_format == 1)  ///< IBM float
        {
            float *sour = (float *)source;
            float *dest = (float *)destination;
            //            for(int i=0; i<length; ++i)
            //            {
            //                dest[i] = ibm32(sour[i]);
            //            }
            ibm_to_float((int*)sour, (int*)dest, length,0);
        }
        else if (m_format == 2) ///< 4 bytes
        {
            int *sour = (int *)source;
            float *dest = (float *)destination;
            for(int i=0; i<length; ++i)
            {
                dest[i] = sour[i];
            }
        }
        else if (m_format == 3) ///< 2 bytes
        {
            short *sour = (short *)source;
            float *dest = (float *)destination;
            for(int i=0; i<length; ++i)
            {
                dest[i] = sour[i];
            }
        }
        else if (m_format == 5) ///< IEEE float
        {
            memcpy(destination, source, length);
        }
        else if (m_format == 8) ///< 1 byte
        {
            char *sour = (char *)source;
            float *dest = (float *)destination;
            for(int i=0; i<length; ++i)
            {
                dest[i] = sour[i];
            }
        }
    }
    else
    {
        if (m_format == 1)  ///< IBM float
        {
            float *sour = (float *)source;
            float *dest = (float *)destination;
            //            for(int i=0; i<length; ++i)
            //            {
            //                char buffer[4];
            //                char *p = (char*)&sour[i];
            //                buffer[0] = p[3];
            //                buffer[1] = p[2];
            //                buffer[2] = p[1];
            //                buffer[3] = p[0];
            //                dest[i] = ibm32(*(float*)buffer);
            //            }
            ibm_to_float((int*)sour, (int*)dest, length,1);
        }
        else if (m_format == 2) ///< 4 bytes
        {
            int *sour = ( int *)source;
            float *dest = (float *)destination;
            for(int i=0; i<length; ++i)
            {
                int value = ( int)sour[i];
                dest[i] = __bswap_constant_32(value);
            }
        }
        else if (m_format == 3) ///< 2 bytes
        {
            short *sour = ( short *)source;
            float *dest = (float *)destination;
            for(int i=0; i<length; ++i)
            {
                short value = ( short)sour[i];
                dest[i] = __bswap_constant_16(value);
            }
        }
        else if (m_format == 5) ///< IEEE float
        {
            float *sour = (float *)source;
            float *dest = (float *)destination;
            for(int i=0; i<length; ++i)
            {
                char buffer[4];
                char *p = (char*)&sour[i];
                buffer[0] = p[3];
                buffer[1] = p[2];
                buffer[2] = p[1];
                buffer[3] = p[0];
                dest[i] = *(float*)buffer;
            }
        }
        else if (m_format == 8) ///< 1 byte
        {
            char *sour = (char *)source;
            float *dest = (float *)destination;
            for(int i=0; i<length; ++i)
            {
                dest[i] = sour[i];
            }
        }
    }
}
