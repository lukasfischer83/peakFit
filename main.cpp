#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Manual Peak Fitter");
    a.setOrganizationName("University of Innsbruck");
    a.setOrganizationDomain("uibk.ac.at");

    MainWindow w;
    w.show();

    return a.exec();
}
