#include "widgetfuelmain.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WidgetFuelMain w;
    w.show();
    return a.exec();
}
