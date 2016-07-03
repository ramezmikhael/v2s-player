#include "frmmain.h"
#include <QApplication>
#include <QStyleFactory>
#include <QSplashScreen>

/*
 * When using a version of Qt that was built with MinGW dwrf exception handling
 * configuration, uncomment the next line.to fool the compiler.
 * Otherwise keep it commented.
 */
//void * __gxx_personality_v0;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));

    QCoreApplication::setApplicationName("VTS Player");
    QCoreApplication::setOrganizationName("VTS Player Inc.");
    QCoreApplication::setApplicationVersion("0.2.4");

    QSplashScreen splash(QPixmap(":/pics/pics/splash.png"));
    splash.show();
    a.processEvents();
    frmMain w;
    w.show();
    w.createThumbBar();
    splash.finish(&w);

    return a.exec();
}
