#include "map.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Map w;
    w.resize(2000, 2000);
    QPalette pal = w.palette();
    pal.setBrush(QPalette::Background, QColor(50, 50, 50));
    w.setPalette(pal);
    QMessageBox* MBox = new QMessageBox;
    MBox->setText("Для построения маршрута нажмите на начальную и конечную точку пути \n\n"
                  "Чтобы поставить метку, введите текст, нажмите 'Ок.' или Enter. Если текст исчез, то удерживайте левую кнопку мыши на карте и ставьте метку");
    w.show();
    MBox->open();
    return a.exec();
}
