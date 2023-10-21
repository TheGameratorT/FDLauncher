#include "gameselectwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
#ifdef _WIN32
    qputenv("QT_QPA_PLATFORM", qgetenv("QT_QPA_PLATFORM") + QByteArray(";windows:darkmode=1"));
#endif

    QApplication a(argc, argv);

    srand(time(NULL));

    QTranslator translator;
    translator.load(QApplication::applicationDirPath() + "/language/FDLauncher_" + QLocale::system().name());
    a.installTranslator(&translator);

	a.setQuitOnLastWindowClosed(false);

    GameSelectWindow w;
    w.show();
    return a.exec();
}
