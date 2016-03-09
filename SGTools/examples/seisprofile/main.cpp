#include <QApplication>

#include <QTextCodec>

#include "profilemainwindow.h"

static inline void supportChinese()
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
}

int main(int argc, char *argv[])
{
    supportChinese();

    QApplication a(argc, argv);

    ProfileMainWindow w;
    w.show();

    return a.exec();
}
