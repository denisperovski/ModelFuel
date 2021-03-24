#pragma once

#include <QtWidgets>
#include <economyfuelclass.h>
#include "qcustomplot.h"
#include <sqlite3.h>

class WidgetFuelMain : public QWidget
{
    Q_OBJECT

public:
    WidgetFuelMain(QWidget *parent = nullptr);

    QPushButton* pushReceiver;
    QMessageBox* MessageBox;
    QTableWidget* TableFuel;//таблица

    QCustomPlot* Plot;//полотно для графиков
    QCPBars* Price;//гистограммы

    vector <tuple <string, double >> fuel;//переменная в которую занесем название топлива и цену из БД

    VectorFuel fuelTop = {{"", 0, 0, 0},
                          {"", 0, 0, 0},
                          {"", 0, 0, 0}};//топ 3 топлив (по цене)

    QVector <QString> Names;
    QVector <double> Rashod, PriceData, ticks;//переменные для гистограммы

public slots:
    void Database();
    void receiver();
    void counter();
    void Bar();//функции для кнопок

private:
    QSharedMemory memName, memRashod;
};//принимаем данные из первой программы
