#include "accountmanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile styleSheetFile(":/style/Combinear.qss");
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String{styleSheetFile.readAll()};
    a.setStyleSheet(styleSheet);
    AccountManager w;
    w.show();
    return a.exec();
}
