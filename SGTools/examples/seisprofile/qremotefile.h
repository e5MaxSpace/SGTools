#ifndef QREMOTEFILE_H
#define QREMOTEFILE_H

#include <QFile>

class IFile
{
public:
    IFile() {}
    virtual ~IFile() {}

    virtual bool open() = 0;
    virtual void close() = 0;
    virtual qint64 read(char *data, qint64 maxlen) = 0;

    virtual qint64 pos() const = 0;
    virtual bool seek(qint64 offset) = 0;
    virtual qint64 size() const = 0;
};

class QLocalFile : public IFile
{
public:
    static bool read(const QString &fileName, int offset, int length, char *buffer);

public:
    explicit QLocalFile(const QString &fileName);
    virtual ~QLocalFile();

    virtual bool open();
    virtual void close();
    virtual qint64 read(char *data, qint64 maxlen);

    virtual qint64 pos() const;
    virtual bool seek(qint64 offset);
    virtual qint64 size() const;

private:
    QFile *m_file;
};

//class QRemoteFile : public IFile
//{
//public:
//    static bool read(const QString &serverIp, const QString &fileName,
//                     int offset, int length, char *buffer);

//public:
//    explicit QRemoteFile(const QString &serverIp, const QString &fileName);
//    virtual ~QRemoteFile();

//    virtual bool open();
//    virtual void close();
//    virtual qint64 read(char *data, qint64 maxlen);

//    virtual qint64 pos() const;
//    virtual bool seek(qint64 offset);
//    virtual qint64 size() const;

//private:
//    const QString m_serverIp;
//    const QString m_fileName;

//    int m_sock_fd;
//    FILE *m_file;
//};

#endif // QREMOTEFILE_H
