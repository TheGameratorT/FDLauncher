#include "gameselectwindow.h"
#include "launcher.h"

#include <QApplication>
#include <QTranslator>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
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
