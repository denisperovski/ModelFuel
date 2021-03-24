#include "map.h"
#include "marker.h"
#include <QtWidgets>
#include <QQuickWidget>
#include <QQuickItem>

void Map::keyPressEvent(QKeyEvent *event)//нажать на интер чтоб кликнуть кнопки либо поиска либо маркера
{
    if (event->type() == QEvent::KeyPress)
    {
        if (TextMarker == QWidget::focusWidget())
            ButtonMarker->click();
        if (TextSearch == QWidget::focusWidget())
            ButtonSearch->click();
    }
}

void Map::buttonclick()
{
    if (ButtonMarker == QObject::sender())//если кнопка была нажата
    {
        QVariant lbl = TextMarker->text().toUtf8().constData();//выносим текст
        QMetaObject::invokeMethod(qRoot, "label", Q_RETURN_ARG(QVariant, lbl), Q_ARG(QVariant, lbl));//отправляем текст в функцию в QML
        TextMarker->clear();//очищаем строку
    }

    if (ButtonHome == QObject::sender())
        QMetaObject::invokeMethod(qRoot, "home");//вызов функции в QML

    if (ButtonSearch == QObject::sender())
    {
        QVariant lbl2 = TextSearch->text().toUtf8().constData();
        QMetaObject::invokeMethod(qRoot, "search", Q_RETURN_ARG(QVariant, lbl2), Q_ARG(QVariant, lbl2));
    }
}

void Map::slotButtonClicked()
{
    if (RadButtonRoute->isChecked())//если радиобатон был нажат
        QMetaObject::invokeMethod(RouteModel, "modelOn");
    else
        QMetaObject::invokeMethod(RouteModel, "modelOff");

    if (RadButtonMarker->isChecked())
    {
        LabelMarker->show();
        TextMarker->show();
        ButtonMarker->show();
        TypeMap->hide();
    }
    else
    {
        LabelMarker->hide();
        TextMarker->hide();
        ButtonMarker->hide();
        TypeMap->show();
    }


    if (ButtonMenu->isChecked())
    {
        RadButtonRoute->show();
        RadButtonMarker->show();
        ButtonHome->show();
    }
    else
    {
        RadButtonRoute->hide();
        RadButtonMarker->hide();
        ButtonHome->hide();
        TextMarker->hide();
        LabelMarker->hide();
        ButtonMarker->hide();
        TypeMap->hide();
    }
}

void Map::comboboxchange(const QString)
{
    QVariant iType = TypeMap->currentIndex();
    QMetaObject::invokeMethod(qRoot, "typeMaps", Q_RETURN_ARG(QVariant, iType), Q_ARG(QVariant, iType));//и отправляем в функции QML
}

Map::Map (QWidget *parent)
{
    Q_UNUSED(parent)
    m_quickWidget = new QQuickWidget(this);

    qmlRegisterType<MarkerModel>("MarkerModel", 1, 0, "MarkerModel");//добавляем класс MarkerModel в QML в качестве библиотеки
    m_quickWidget->setSource(QUrl("qrc:/main.qml"));//загружаем компонент QML

    qRoot = m_quickWidget->rootObject();//"заходим" в главный класс QML, то есть в Item
    if (qRoot)
        RouteModel = qRoot->findChild<QObject*>("routeModel");//те же махинации, только заходим в класс routeModel


//---------------инициализируем переменные-----------
    RadButtonRoute = new QRadioButton("Маршрут");
    RadButtonMarker = new QRadioButton("Метка");

    ButtonMenu = new QCheckBox;

    TypeMap = new QComboBox;

    QStringList listTypeCB;
    QStringList listModsCB;

    ButtonMarker = new QPushButton("Ок.");
    ButtonSearch = new QPushButton("Поиск");
    ButtonHome = new QPushButton("Домой");

    TextMarker = new QLineEdit;
    TextSearch = new QLineEdit;

    LabelMarker = new QLabel("Оставьте отзыв или напишите заметку о посещенном месте.");
//---------------------------------------------------

    //добавляем в кукомбы список карт и типы передвижения
    listTypeCB << "Пустая карта" << "Карта улиц" << "Дневная карта" << "Ночное рандеву" << "Карта местности" << "Прогулочные маршруты" << "Полная карта";
    TypeMap->addItems(listTypeCB);
    TypeMap->setCurrentIndex(6);//ставим дефолтным 7 тип карты
    listTypeCB.clear();

    //активируем данные переключатели
    ButtonMenu->setChecked(true);
    RadButtonRoute->setChecked(true);

    //красивые кнопки
    ButtonHome->setStyleSheet({"QPushButton{" "background-color: rgb(255, 255, 255);" "color: rgb(0, 0, 0);}" "QPushButton:hover{"
                               "background-color: rgb(232, 121, 62);}" "QPushButton:pressed  {" "background-color: rgb(227, 100, 102);}"});
    ButtonHome->setCursor(Qt::PointingHandCursor);

    ButtonMarker->setStyleSheet({"QPushButton{" "background-color: rgb(255, 255, 255);" "color: rgb(0, 0, 0);}" "QPushButton:hover{"
                               "background-color: rgb(232, 121, 62);}" "QPushButton:pressed  {" "background-color: rgb(227, 100, 102);}"});
    ButtonMarker->setCursor(Qt::PointingHandCursor);

    ButtonSearch->setStyleSheet({"QPushButton{" "background-color: rgb(255, 255, 255);" "color: rgb(0, 0, 0);}" "QPushButton:hover{"
                               "background-color: rgb(232, 121, 62);}" "QPushButton:pressed  {" "background-color: rgb(227, 100, 102);}"});
    ButtonSearch->setCursor(Qt::PointingHandCursor);

    //кастомный чекбокс
    ButtonMenu->setStyleSheet("QCheckBox::indicator:checked{image: url(:/image/outmain.png);}"
                              "QCheckBox::indicator:unchecked{image: url(:/image/inmain.png);}"
                              "QCheckBox::indicator:unchecked:pressed {image: url(:/image/clickinmain.png);}"
                              "QCheckBox::indicator:checked:pressed {image: url(:/image/clickoutmain.png);}"
                              "QCheckBox::indicator{width: 40px; height: 40px;}");
    ButtonMenu->setCursor(Qt::PointingHandCursor);

    //коннектим все нажатия, переключения и прочее в слоты
    connect(TypeMap, SIGNAL(currentTextChanged(const QString)), SLOT(comboboxchange(const QString)));
    connect(RadButtonRoute, SIGNAL(clicked()), SLOT(slotButtonClicked()));
    connect(RadButtonMarker, SIGNAL(clicked()), SLOT(slotButtonClicked()));
    connect(ButtonMenu, SIGNAL(clicked()), SLOT(slotButtonClicked()));
    connect(ButtonMarker, SIGNAL(clicked()), SLOT(buttonclick()));
    connect(ButtonHome, SIGNAL(clicked()), SLOT(buttonclick()));
    connect(ButtonSearch, SIGNAL(clicked()), SLOT(buttonclick()));

    //интерфейсная косметика
    TextSearch->setPlaceholderText("Поиск мест и адресов");
    LabelMarker->setBuddy(TextMarker);

    QPalette pal;
    pal.setColor(QPalette::Base, QColor(31, 31, 31));
    TextSearch->setPalette(pal);
    TextMarker->setPalette(pal);

    LabelMarker->setStyleSheet("QLabel {color: rgb(255, 255, 255)}");

    RadButtonRoute->setStyleSheet("QRadioButton {color: rgb(255, 255, 255);}"
                                  "QRadioButton::indicator:checked{image: url(:/image/on.png);}"
                                  "QRadioButton::indicator:unchecked{image: url(:/image/off.png);}"
                                  "QRadioButton::indicator{width: 40px; height: 30px;}");
    RadButtonMarker->setStyleSheet("QRadioButton {color: rgb(255, 255, 255);}"
                                   "QRadioButton::indicator:checked{image: url(:/image/on.png);}"
                                   "QRadioButton::indicator:unchecked{image: url(:/image/off.png);}"
                                   "QRadioButton::indicator{width: 40px; height: 30px;}");

    //скрываем объекты, связанные с меткой, так как по дефолту стоит маршрут
    TextMarker->hide();
    LabelMarker->hide();
    ButtonMarker->hide();

//--------------добавляем все объекты на виджет---------------
    QGridLayout* buttons = new QGridLayout;//макет сетки, чтобы кнопки уместились в квадратик
    buttons->addWidget(RadButtonRoute, 0, 0, Qt::AlignLeft);
    buttons->addWidget(RadButtonMarker, 1, 0, Qt::AlignLeft);
    buttons->addWidget(ButtonMenu, 0, 1, Qt::AlignRight);
    buttons->addWidget(ButtonHome, 1, 1, Qt::AlignRight);
    buttons->setSpacing(15);//расстояние между кнопками

    QHBoxLayout* qhbMarker = new QHBoxLayout;//горизонтальный макет, т.е. добавляет объекты слева направо
    qhbMarker->addWidget(TextMarker);
    qhbMarker->addWidget(ButtonMarker);

    QHBoxLayout* qhbSearch = new QHBoxLayout;
    qhbSearch->addWidget(TextSearch);
    qhbSearch->addWidget(ButtonSearch);

    QVBoxLayout* qvb = new QVBoxLayout;//вертикальный макет, аналогично, только сверху вниз
    qvb->addLayout(buttons);
    qvb->addStretch(1);//ставим коэффициент растяжения
    qvb->addWidget(TypeMap);
    qvb->addWidget(LabelMarker);
    qvb->addLayout(qhbMarker);
    qvb->addStretch(1);

    QHBoxLayout* qhb = new QHBoxLayout;
    qhb->addWidget(m_quickWidget);
    qhb->addLayout(qvb);

    QVBoxLayout* qvbfinal = new QVBoxLayout;
    qvbfinal->addLayout(qhbSearch);
    qvbfinal->addLayout(qhb);
    setLayout(qvbfinal);//выводим в виджет
//------------------------------------------------------------
    slotButtonClicked();
}
