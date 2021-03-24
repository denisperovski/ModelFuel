#include "widgetfuelmain.h"
#include "QtSql/QSqlDatabase"
#include "QSqlQuery"

void WidgetFuelMain::Database()
{
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbname = "/databaseFuel";
    QString pathtoDB = QDir::currentPath()+QString("/%1.db3").arg(dbname);
    db.setDatabaseName(pathtoDB);
    db.open();//получаем данные из БД

    QSqlQuery query;
    query.exec("SELECT Count(*) From Fuel");
    query.next();
    int k = query.value(0).toInt();//запрашиваем количество топлива из БД

    fuel.resize(k);

    query.exec("SELECT Name, Price from Fuel");
    int i = 0;

    while (query.next())
    {
        get<0>(fuel[i]) = query.value(0).toString().toStdString();
        get<1>(fuel[i]) = query.value(1).toDouble();
        i++;
    }
    db.close();//выводим название и цену топлива
}

void Economy(VectorFuel &fuel)
{
    unsigned int nFuel = static_cast<unsigned int>(fuel.size());

    EconomyFuelClass EcFuel;

    for (unsigned int i = 0; i<nFuel; i++)
        get<3>(fuel[i]) = EcFuel.ResultPrice(get<1>(fuel[i]), get<2>(fuel[i]));//выводим стоимость за весь полет

    EcFuel.ResultSort(fuel, nFuel);//сортируем методом пузырька
}

void WidgetFuelMain::Bar()
{
    Plot->setBackground(QColor(236, 236, 236));

    Price = new QCPBars(Plot->xAxis, Plot->yAxis);

    Price->setAntialiased(false);
    Price->setStackingGap(1);//полотно для гистрограмм

    Plot->xAxis->setRange(0, 4);
    Plot->xAxis->setTickLabelRotation(30);
    Plot->xAxis->setTickLength(0, 4);//(тики это палки сверху названий) направляет их вниз
    Plot->xAxis->setBasePen(QPen(Qt::black));
    Plot->xAxis->setTickPen(QPen(Qt::black));//делает белыми
    Plot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    Plot->xAxis->setTickLabelColor(Qt::black);//цвет названий
    Plot->xAxis->setLabel("Топливо");
    Plot->xAxis->setLabelColor(Qt::black);

    Plot->yAxis->setPadding(5); // расстояние от лэйбла до оси
    Plot->yAxis->setBasePen(QPen(Qt::black));
    Plot->yAxis->setTickPen(QPen(Qt::black));
    Plot->yAxis->setSubTickPen(QPen(Qt::black));
    Plot->yAxis->grid()->setSubGridVisible(true);
    Plot->yAxis->setTickLabelColor(Qt::black);
    Plot->yAxis->setLabelColor(Qt::black);
    Plot->yAxis->grid()->setPen(QPen(Qt::black, 0, Qt::SolidLine));
    Plot->yAxis->grid()->setSubGridPen(QPen(Qt::black, 0, Qt::DotLine));
}

void WidgetFuelMain::counter()
{
    resize(600, 600);//размер окна
    int nFuel = static_cast<int>(fuel.size());//считает кол-во топлива

    for (int i = 0; i < Names.size(); i++)
    {
        get<0>(fuelTop[i]) = Names[i].toStdString();//заносим расход и имена топлива в fueltop
        get<1>(fuelTop[i]) = Rashod[i];
    }

    int nFuelTop = static_cast<int>(fuelTop.size());

    for (int i = 0; i < nFuel; i++)
        for (int j = 0; j < nFuelTop; j++)
            if (get<0>(fuel[i]) == get<0>(fuelTop[j]))
                get<2>(fuelTop[j]) = get<1>(fuel[i]);//сравниваем названия топлив из БД и первой программы и заносим цену за литр

    Economy(fuelTop);

    TableFuel->clear();//очищаем таблицу

    TableFuel->setHorizontalHeaderLabels(QStringList() << "Название топлива" << "Расход топлива, кг" << "Цена за кг, руб" << "Общая сумма");
    TableFuel->setFixedSize(461, 112);//таблица

    for (unsigned int i = 0; i < 3; i++)
    {
        int k = static_cast<int> (i);
        TableFuel->setItem(k, 0, new QTableWidgetItem(QString::fromStdString(get<0>(fuelTop[i]))));
        TableFuel->setItem(k, 1, new QTableWidgetItem(QString::number(get<1>(fuelTop[i]))));
        TableFuel->setItem(k, 2, new QTableWidgetItem(QString::number(get<2>(fuelTop[i]))));
        TableFuel->setItem(k, 3, new QTableWidgetItem(QString::number(get<3>(fuelTop[i]))));//заносим данные в таблицу
    }
    TableFuel->resizeColumnsToContents();//размер ячейки в зависимости от длины слова

    for (unsigned int i = 0; i < 4; i++)
        TableFuel->item(0, i)->setBackground(QColor(0, 250, 154));//первую строку красим в зеленый

    Plot->clearPlottables();
    PriceData.clear();
    Bar();//очищаем и заново задаем полотно

    for (int i = 0; i < nFuelTop; i++)
        PriceData.append(get<3>(fuelTop[i]));//добавляем стоимость за весь полет в переменную

    Plot->yAxis->setRange(0, PriceData[2] * 1.1);//высота полотна

    Price->setPen(QPen(QColor(0, 191, 255).lighter(150)));
    Price->setBrush(QColor(0, 191, 255));//цвета гистограмм
    Plot->yAxis->setLabel("Цена, руб.");//название оси

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, Names);
    Plot->xAxis->setTicker(textTicker);//добавляем имена на полотно

    Price->setData(ticks, PriceData);
    Plot->replot();//сами гистограммы по цене

    TableFuel->show();
    Plot->show();//выводи на экран
}

void WidgetFuelMain::receiver()//принимаем данные из первой программы
{
    memName.setKey("KeyForName");

    if (!memName.attach())
    {
        MessageBox->setText(tr("Невозможно подключиться к сегменту общей памяти."));
        return;
    }

    QBuffer bufferName;
    QDataStream inName(&bufferName);

    memName.lock();
    bufferName.setData((char*)memName.constData(), memName.size());
    bufferName.open(QBuffer::ReadOnly);
    inName >> Names;//сюда приняли имена топ 3 топлива из 1 программы
    memName.unlock();
    memName.detach();
//--------------------------------------
    memRashod.setKey("KeyForRashod");

    if (!memRashod.attach())
    {
        MessageBox->setText(tr("Невозможно подключиться к сегменту общей памяти."));
        return;
    }

    QBuffer bufferRashod;
    QDataStream inRashod(&bufferRashod);

    memRashod.lock();
    bufferRashod.setData((char*)memRashod.constData(), memRashod.size());
    bufferRashod.open(QBuffer::ReadOnly);
    inRashod >> Rashod;//сюда приняли расход топ 3 топлива из 1 программы
    memRashod.unlock();
    memRashod.detach();
}

WidgetFuelMain::WidgetFuelMain(QWidget *parent)
{
    Q_UNUSED(parent)

    Database();
    ticks << 1 << 2 << 3;

    pushReceiver = new QPushButton("Получить данные");
    MessageBox = new QMessageBox;
    TableFuel = new QTableWidget(3, 4);//таблица 3 на 4
    Plot = new QCustomPlot;//полотно для графиков

    pushReceiver->setStyleSheet("QPushButton {" "border-radius: 10px; padding: 6px; background-color: rgb(128, 0, 0);" "color: rgb(255, 255, 255);}"
                               "QPushButton:hover {" "background-color: rgb(165, 42, 42);}"
                               "QPushButton:pressed {" "background-color: rgb(110, 0, 0); } ");//косметика для кнопки
    pushReceiver->setCursor(Qt::PointingHandCursor);//изменение курсора при наводе на кнопку

    connect (pushReceiver, &QPushButton::clicked, this, &WidgetFuelMain::receiver);
    connect (pushReceiver, &QPushButton::clicked, this, &WidgetFuelMain::counter);//вызываем функции при нажатии на кнопку

    TableFuel->hide();
    Plot->hide();//скрываем пустые таблицы и графики

    QVBoxLayout *qvb = new QVBoxLayout;
    qvb->addWidget(TableFuel, 0, Qt::AlignHCenter);
    qvb->addWidget(Plot);
    qvb->addWidget(pushReceiver);//макет виджетов

    setLayout(qvb);//выводим на экран
}


