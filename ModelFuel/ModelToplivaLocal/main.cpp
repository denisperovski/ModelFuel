#include "widgetfuellocal.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WidgetFuelLocal w;
    w.show();
    return a.exec();
}
