#include "mainwindow.h"
#include <QApplication>
//#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

//    QThread gobanThread;
//    Goban boardObject;

//    boardObject.moveToThread(&gobanThread);
//    gobanThread.start(QThread::NormalPriority);

    w.show();

    return a.exec();
}
