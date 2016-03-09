#include "qremotefile.h"

//#include "SrcSocketNet.h"
//#include "SrcRemoteFileServer.h"
//#include "SrcRemoteFileServer_clnt.h"

#include <QFile>
#include <QDebug>

QLocalFile::QLocalFile(const QString &fileName) :
    m_file(new QFile(fileName))
{
}

QLocalFile::~QLocalFile()
{
    delete m_file;
    m_file = 0;
}

bool QLocalFile::open()
{
    return m_file->open(QIODevice::ReadOnly);
}

void QLocalFile::close()
{
    m_file->close();
}

qint64 QLocalFile::read(char *data, qint64 maxlen)
{
    return m_file->read(data, maxlen);
}

qint64 QLocalFile::pos() const
{
    return m_file->pos();
}

bool QLocalFile::seek(qint64 offset)
{
    return m_file->seek(offset);
}

qint64 QLocalFile::size() const
{
    return m_file->size();
}

bool QLocalFile::read(const QString &fileName, int offset, int length, char *buffer)
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

// -------------------------------------
//QRemoteFile::QRemoteFile(const QString &serverIp, const QString &fileName) :
//    m_serverIp(serverIp), m_fileName(fileName), m_file(0)
//{
//}

//QRemoteFile::~QRemoteFile()
//{
//    close();
//}

//bool QRemoteFile::open()
//{
//    m_sock_fd = SrcGetServer_Clnt(SRC_FILE_OP_PORT, m_serverIp.toAscii().data());
//    qDebug()<<"m_sock_fd="<<m_sock_fd;
//    m_file = SrcRfopen64(m_sock_fd, m_fileName.toAscii().data(), "rb+");
//    if (m_file == 0)
//    {
//        qWarning()<<Q_FUNC_INFO<<"SrcRfopen64() failed. fileName="<<m_fileName;
//        return false;
//    }

//    return true;
//}


//void QRemoteFile::close()
//{
//    if (m_file)
//    {
//        SrcRfclose(m_sock_fd, m_file);
//        SrcCloseSocket(m_sock_fd);
//    }
//}

//qint64 QRemoteFile::read(char *data, qint64 maxlen)
//{
//#ifdef QT_DEBUG
//    if (!m_file)
//    {
//        qWarning()<<Q_FUNC_INFO<<"error! m_file is null. please first call open.";
//        return 0;
//    }
//#endif

//    char tempData[maxlen*10];
//    size_t ret = SrcRfread(m_sock_fd, tempData, 1, maxlen*10, m_file);
//    memcpy(data, tempData, maxlen);

//#ifdef QT_DEBUG
//    qDebug()<<"QRemoteFile::read(). ret="<<ret;
//#endif

//    return ret;
//}

//qint64 QRemoteFile::pos() const
//{
//    if (m_file)
//    {
//        return SrcRftell64(m_sock_fd, m_file);
//    }

//    return 0;
//}

//bool QRemoteFile::seek(qint64 offset)
//{
//#ifdef QT_DEBUG
//    if (!m_file)
//    {
//        qWarning()<<Q_FUNC_INFO<<"error! m_file is null. please first call open.";
//        return false;
//    }
//#endif

//    int ret = SrcRfseek64(m_sock_fd, m_file, offset, 0);

//    return (ret == 0);
//}

//qint64 QRemoteFile::size() const
//{
//    long fsize = 0L;
//    if (m_file)
//    {
//        SrcRfseek64(m_sock_fd, m_file, 0, 2);
//        fsize = SrcRftell64(m_sock_fd, m_file);
//        SrcRfrewind(m_sock_fd, m_file);
//    }

//    return fsize;
//}

//bool QRemoteFile::read(const QString &serverIp, const QString &fileName,
//                       int offset, int length, char *buffer)
//{
//    QRemoteFile segyfile(serverIp, fileName);
//    if (!segyfile.open())
//    {
//        qWarning()<<Q_FUNC_INFO<<"Open file failed. fileName="<<fileName;
//        return false;
//    }

//    segyfile.seek(offset);
//    segyfile.read(buffer, length);
//    return true;
//}
