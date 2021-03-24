#pragma once

#include <QtWidgets>
#include <ecologyfuelclass.h>
#include "qcustomplot.h"

class WidgetFuelLocal : public QWidget
{
    Q_OBJECT

public:
    WidgetFuelLocal(QWidget *parent = nullptr);
//объявление виджетов
    QComboBox* ComboBoxPPA;
    QLineEdit* Distance;
    QPushButton* push;
    QPushButton* pushCourier;
    QRadioButton* RadButtPercent;
    QRadioButton* RadButtRashod;
    QRadioButton* RadButtMass;
    QTableWidget* TableFuel;
    QMessageBox* MessageBox;
//гистограмм
    QCustomPlot* Plot;
    QCPBars* MassEmissions;
    QCPBars* Consumption;
    QCPBars* PercentCH;
    QVector <double> ticks, MassEmissionsData, ConsumptionData, PercentData;
    QVector <QString> labels;
//переменные в которых будет храниться топливо и СУЛА и их хар-ки
    VectorFuel fuel;
    VectorFuel fuell = {{"", 0, 0, 0, 0},
                        {"", 0, 0, 0, 0},
                        {"", 0, 0, 0, 0}};
    VectorPpa ppa;

    double Dlina = 1;
    int k = 0;
//функции для вызова по кнопкам
public slots:
    void courier();
    void buttonclick();
    void RadButtonChange();
    void DatabaseFuel();
    void DatabasePPA();
//функция задающее фон гистограммы
private slots:
    void Bar();
//для передачи в другую программу
private:
    QSharedMemory memName, memRashod;

};
