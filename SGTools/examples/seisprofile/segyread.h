#ifndef SEGYREAD_H
#define SEGYREAD_H

#include <QObject>
#include <QFile>


enum SegyDim
{
    _2D,
    _3D
};

enum
{
    fileHeaderPrefixLength = 3200,  ///< 文件头前长度
    fileHeaderSuffixLength = 400,   ///< 文件头尾长度
    fileHeaderLength = fileHeaderPrefixLength+fileHeaderSuffixLength,  ///< 文件头总长度

    traceHeaderLength = 240,    ///< 道头前长度
};

class SegyRead : public QObject
{
    Q_OBJECT

public:
    enum IncrementDirection
    {
        InlineIncrement,
        XlineIncrement
    };

    struct SegyPoint
    {
        int line;
        int cdp;
        float x;
        float y;
    };

public:
    /** @brief 读取SEG-Y文件
     *
     *  @param filePath - 要读取的文件路径（包括文件名）
     *  @param offset - 文件读取偏移
     *  @param length - 要读取的文件长度（字节数）
     *  @param buffer - 数据缓冲区的指针，用于返回数据
     *  @return - 返回是否读取成功
     */
    static bool read(const QString &fileName, int offset, int length, char *buffer);

    /** @brief 截取指定位置和长度的值
     *
     *  @param p - 开始读取的指针
     *  @param len - 读取数据长度
     *  @param isBigEndian - 是否大端模式
     *  @return - 返回整型值
     */
    static int mid(char *buffer, int len, bool isBigEndian);

    /** @brief 更新指定位置和长度的值
     *
     *  @param p - 开始读取的指针
     *  @param len - 读取数据长度
     *  @param isBigEndian - 是否大端模式
     *  @param value - 要更新的值
     */
    static void set(char *p, int len, bool isBigEndian, int value);

    /** @brief 样本格式值转换到字节数
     *
     *  @param format - 样本格式值
     *  @return 格式所占字节数
     */
    static int sampleFormat2Bytes(int format);

public:
    explicit SegyRead(QObject *parent = 0);

    /** @brief 设置数据格式
     */
    void setFormat(int format)
    {
        m_format = format;
    }

    /** @brief 设置采样点数
     */
    void setSampleNumber(int sampleNumber)
    {
        m_sampleNumber = sampleNumber;
    }

    /** @brief 设置数据大小端
     */
    void setBigEndian(bool isBigEndian)
    {
        m_isBigEndian = isBigEndian;
    }

    /** @brief 设置线道号相对于道头的偏移
     */
    void setOffset(int lineOffset, int lineLength, int cdpOffset, int cdpLength)
    {
        m_lineOffset = lineOffset;
        m_lineLength = lineLength;
        m_cdpOffset = cdpOffset;
        m_cdpLength = cdpLength;
    }

    /** @brief 设置第一个采样点对应的时间
     */
    void setBeginTime_us(int time_us)
    {
        m_beginTime_us = time_us;
    }

    /** @brief 打开文件
     */
    int open(const QString &fileName);

    /** @brief 从指定范围beginLine —— endLine 按文件顺序读取一组数据
     *
     *  @param beginLine - 开始线号
     *  @param endLine - 结束线号
     *  @param beginCdp - 开始道号
     *  @param endCdp - 结束道号
     *  @param cdpIncrement - 道读取增量
     *  @param beginTime - 开始道号
     *  @param endTime - 结束道号
     *  @param timeIncrement - 道读取增量
     *  @param currentLine - 当前线号
     *  @param data - 存放读取数据的缓冲区
     *  @param percent - 完成百分比
     *  @return 0 if successful
     */
    int readLine(int beginLine, int endLine,
                 int beginCdp, int endCdp, int cdpIncrement,
                 int beginTime,int endTime, int timeIncrement,
                 int &currentLine, QVector<float> &data, int &percent);

    /** @brief 从指定范围beginCdp —— endCdp 按文件顺序读取一组数据
     *
     *  @param beginCdp - 开始道号
     *  @param endCdp - 结束道号
     *  @param beginLine - 开始线号
     *  @param endLine - 结束线号
     *  @param cdpIncrement - 道读取增量
     *  @param beginTime - 开始道号
     *  @param endTime - 结束道号
     *  @param timeIncrement - 道读取增量
     *  @param currentLine - 当前线号
     *  @param data - 存放读取数据的缓冲区
     *  @param percent - 完成百分比
     *  @return 0 if successful
     */
    int readCdp(int beginCdp, int endCdp,
                int beginLine, int endLine, int lineIncrement,
                int beginTime, int endTime, int timeIncrement,
                int &currentLine, QVector<float> &data, int &percent);


    /** @brief 获取3个点，分别取头尾和中间
     *
     *  @return 0 if successful
     */
    void get3Points(int xOffset, int xLength, int yOffset, int yLength,
                    SegyPoint &p1, SegyPoint &p2, SegyPoint &p3);

    /** @brief 外循环是否是线方向递增
     */
    bool isInlineIncrement()
    {
        return (m_incrementDirection == InlineIncrement);
    }

    /** @brief 二进制数据转换
     */
    void binaryConvert(void *destination, void *source, int length);

    void readTraceValue(qint64 offset, int length, int &value);

private:
    /** @brief 数据格式 */
    int m_format;

    /** @brief 数据存放是大端？ */
    bool m_isBigEndian;

    /** @brief 线号相对于道头的偏移 */
    int m_lineOffset;

    /** @brief 线号值字节数 */
    int m_lineLength;

    /** @brief 道号相对于道头的偏移 */
    int m_cdpOffset;

    /** @brief 道号值字节数 */
    int m_cdpLength;

    /** @brief 道头长度 (ms)*/
    int m_beginTime_us;

    /** @brief 道体长度 */
    int m_traceLength;

    /** @brief 采样点数 */
    int m_sampleNumber;

    /** @brief 文件流 */
    QFile m_filestream;

    /** @brief  外循环递增方向 */
    IncrementDirection m_incrementDirection;

    /** @brief 记录当前线号
     *
     *  @note 如何m_incrementDirection == InlineIncrement，m_currentLine记录的是道号
     */
    int m_currentLine;

    /** @brief 记录当前线号的位置 */
    qint64 m_currentLinePos;
};

#endif // SEGYREAD_H
