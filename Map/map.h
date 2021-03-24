#pragma once

#include "marker.h"
#include <QtWidgets>
#include <QQuickWidget>

class Map : public QWidget//основной класс, отоброжающий интерфейс
{
    Q_OBJECT

public:
    Map (QWidget *parent = nullptr);

    //задаем классовые переменные для их действия во всех функциях класса
    QObject* RouteModel;

    QRadioButton* RadButtonRoute;
    QRadioButton* RadButtonMarker;

    QCheckBox* ButtonMenu;

    QComboBox* TypeMap;

    QLineEdit* TextMarker;
    QLineEdit* TextSearch;
    QLabel* LabelMarker;

    QPushButton* ButtonMarker;
    QPushButton* ButtonHome;
    QPushButton* ButtonSearch;

    QQuickItem* qRoot;

private:
    QQuickWidget* m_quickWidget;

protected:
    void keyPressEvent(QKeyEvent *event) override;

public slots:
    void buttonclick();
    void slotButtonClicked();
    void comboboxchange(const QString);
};
