#include "widgetfuellocal.h"
#include "QtSql/QSqlDatabase"
#include "QSqlQuery"

void WidgetFuelLocal::DatabaseFuel()//функция которая берет данные из базы по топливу
{
    QSqlDatabase dbFuel;
    dbFuel = QSqlDatabase::addDatabase("QSQLITE");
    QString dbname = "databaseFuel";
    QString pathtoDB = QDir::currentPath()+QString("/%1.db3").arg(dbname);
    qDebug() << pathtoDB;
    dbFuel.setDatabaseName("/Users/denis/Desktop/databaseFuel.db3");

    dbFuel.open();
    //Осуществляем запрос
    QSqlQuery query;
    query.exec("SELECT Count(*) From Fuel");
    query.next();
    int k = query.value(0).toInt();
    fuel.resize(k);

    query.exec("SELECT * from Fuel");
    int i = 0;
    //Выводим значения из запроса
    while (query.next())
    {
        get<0>(fuel[i]) = query.value(0).toString().toStdString();
        get<1>(fuel[i]) = query.value(1).toDouble();
        get<2>(fuel[i]) = query.value(2).toDouble();
        i++;
    }
    dbFuel.close();
}

void WidgetFuelLocal::DatabasePPA()//то же, но из базы СУЛА
{
    QSqlDatabase dbPPA;
    dbPPA = QSqlDatabase::addDatabase("QSQLITE");
    QString dbname = "/databasePPA";
    QString pathtoDB = QDir::currentPath()+QString("/%1.db3").arg(dbname);
    dbPPA.setDatabaseName("/Users/denis/Desktop/databasePPA.db3");

    dbPPA.open();
    QSqlQuery query;
    query.exec("SELECT Count(*) From PPA");
    query.next();
    int k = query.value(0).toInt();
    ppa.resize(k);

    query.exec("SELECT * from PPA");
    int i = 0;
    while (query.next())
    {
        get<0>(ppa[i]) = query.value(0).toString().toStdString();
        get<1>(ppa[i]) = query.value(1).toDouble();
        get<2>(ppa[i]) = query.value(2).toDouble();
        get<3>(ppa[i]) = query.value(3).toDouble();
        i++;
    }
    dbPPA.close();
}

void Ecology(VectorFuel fuel, VectorPpa ppa, unsigned int k, double Dlina, VectorFuel &fuell)
{
    unsigned int count = 0;//счетчик
    unsigned int nFuel = static_cast<unsigned int>(fuel.size());//кол-во топлива

    char Zppa = get<0>(ppa[k])[0];//первая буква СУЛА, для определения топлива на данную сула

    EcologyFuelClass OptFuel{get<1>(ppa[k]), get<2>(ppa[k]), Dlina, get<3>(ppa[k])};//вызываем класс с формулами

    if ((int(Zppa) > 0) and (get<3>(ppa[k]) < 1500))//если топливо американское и СУЛА дозвуковое
    {
        unsigned int l = 0;//счетчик для удаления топлива

        for (unsigned int i = 0; i < nFuel; i++)
        {
            char Namefuelbegin = get<0>(fuel[l])[0];//первая буква топлива
            char Namefuelend = get<0>(fuel[l])[get<0>(fuel[l]).size() - 1];//последняя буква топлива

            if (int(Namefuelbegin) > 0 and Namefuelend != 's')//если первая буква положительна(то есть американская) и последняя буква не s
                l++;//оставляем топливо и переходим к следующему
            else
            {
                fuel.erase(fuel.begin() + l);//иначе удаляем его
                count++;
            }
        }

        nFuel = nFuel - count;//убираем удаленное кол-во топлива

        for (unsigned int i = 0; i < nFuel; i++)
        {
            get<3>(fuel[i]) = OptFuel.FuelConsumption(get<1>(fuel[i]));//ищем расход данного топлива и добавляем в вектор
            get<4>(fuel[i]) = OptFuel.MassOfEmissions();// то же с массой выбросов
        }
    }
    else if (get<3>(ppa[k]) > 1500)//если СУЛА сверхзвуковое
    {
        unsigned int l = 0;

        for (unsigned int i = 0; i < nFuel; i++)
        {
            char Namefuelbegin = get<0>(fuel[l])[0];
            char Namefuelend = get<0>(fuel[l])[get<0>(fuel[l]).size() - 1];

            if ((Namefuelend == 's') or (int(Namefuelbegin) < 58 and int(Namefuelbegin) > 47))//сверхзвук. топливо обозначается буквой s в конце
                l++;
            else
            {
                fuel.erase(fuel.begin() + l);
                count++;
            }
        }

        nFuel = nFuel - count;

        for (unsigned int i = 0; i < nFuel; i++)
        {
            get<3>(fuel[i]) = OptFuel.FuelConsumption(get<1>(fuel[i]));
            get<4>(fuel[i]) = OptFuel.MassOfEmissions();
        }
    }
    else//для российской СУЛА
    {
        unsigned int l = 0;

        for (unsigned int i = 0; i < nFuel; i++)
        {
            char Namefuelbegin = get<0>(fuel[l])[0];
            char Namefuelend = get<0>(fuel[l])[get<0>(fuel[l]).size() - 1];

            if (((int(Namefuelbegin) < 0) or (int(Namefuelbegin) < 58 and int(Namefuelbegin) > 47)) and int(Namefuelend) != 115)
                l++;
            else
            {
                fuel.erase(fuel.begin() + l);
                count++;
            }
        }

        nFuel = nFuel - count;

        for (unsigned int i = 0; i < nFuel; i++)
        {
            get<3>(fuel[i]) = OptFuel.FuelConsumption(get<1>(fuel[i]));
            get<4>(fuel[i]) = OptFuel.MassOfEmissions();
        }
    }

    OptFuel.ResultSort(fuel, nFuel);//сортируем топливо по процентам и массе выбросов

    for (unsigned int i = 0; i < nFuel; i++)//заносим в глобальное топливо fuell, так как оно изменяемо
    {
        get<0>(fuell[i]) = get<0>(fuel[i]);
        get<1>(fuell[i]) = get<2>(fuel[i]);
        get<2>(fuell[i]) = get<3>(fuel[i]);
        get<3>(fuell[i]) = get<4>(fuel[i]);
    }
}

void WidgetFuelLocal::Bar()//полотно для графиков
{
    Plot->setBackground(QColor(236, 236, 236));//цвет фона

    //задаем гисторграммы
    PercentCH = new QCPBars(Plot->xAxis, Plot->yAxis);
    MassEmissions = new QCPBars(Plot->xAxis, Plot->yAxis);
    Consumption = new QCPBars(Plot->xAxis, Plot->yAxis);

    //куча косметики
    PercentCH->setAntialiased(false);
    Consumption->setAntialiased(false);
    MassEmissions->setAntialiased(false);
    PercentCH->setStackingGap(1);
    Consumption->setStackingGap(1);
    MassEmissions->setStackingGap(1);

    Plot->xAxis->setRange(0, 4);//ось 0Х от 0 до 4
    Plot->xAxis->setTickLabelRotation(30);//угол названий 30 град
    Plot->xAxis->setTickLength(0, 4);//(тики это палки сверху названий) направляет их вниз
    Plot->xAxis->setBasePen(QPen(Qt::black));
    Plot->xAxis->setTickPen(QPen(Qt::black));//делает черным
    Plot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    Plot->xAxis->setTickLabelColor(Qt::black);//цвет названий
    Plot->xAxis->setLabel("Топливо");
    Plot->xAxis->setLabelColor(Qt::black);

    Plot->yAxis->setPadding(5); // расстояние от лэйбла до оси 0У
    Plot->yAxis->setBasePen(QPen(Qt::black));
    Plot->yAxis->setTickPen(QPen(Qt::black));
    Plot->yAxis->setSubTickPen(QPen(Qt::black));
    Plot->yAxis->grid()->setSubGridVisible(true);//сетка
    Plot->yAxis->setTickLabelColor(Qt::black);
    Plot->yAxis->setLabelColor(Qt::black);
    Plot->yAxis->grid()->setPen(QPen(Qt::black, 0, Qt::SolidLine));
    Plot->yAxis->grid()->setSubGridPen(QPen(Qt::black, 0, Qt::DotLine));
}

void WidgetFuelLocal::buttonclick()
{
    //обновляем радиобатоны и включаем кнопку процентов
    RadButtPercent->setChecked(true);
    RadButtRashod->setChecked(false);
    RadButtRashod->repaint();
    RadButtMass->setChecked(false);
    RadButtMass->repaint();
    resize(600, 700);//размер окна

    k = ComboBoxPPA->currentIndex();//считываем индекс
    Dlina = Distance->text().toDouble();//расстояние
    Ecology(fuel, ppa, static_cast<unsigned int>(k), Dlina, fuell);//вызов функции

    //очищаем все для возможности повторить действия
    labels.clear();
    PercentData.clear();
    ConsumptionData.clear();
    MassEmissionsData.clear();
    TableFuel->clear();

    //заполняем табилцу
    TableFuel->setHorizontalHeaderLabels(QStringList() << "Название топлива" << "Cодержание углеводородов, %" << "Расход топлива, кг" << "Масса выбросов, кг");
    TableFuel->setFixedSize(596, 112);

    for (unsigned int i = 0; i < 3; i++)//заносим в таблицу данные
    {
        int k = static_cast<int> (i);
        TableFuel->setItem(k, 0, new QTableWidgetItem(QString::fromStdString(get<0>(fuell[i]))));
        TableFuel->setItem(k, 1, new QTableWidgetItem(QString::number(get<1>(fuell[i]))));
        TableFuel->setItem(k, 2, new QTableWidgetItem(QString::number(get<2>(fuell[i]))));
        TableFuel->setItem(k, 3, new QTableWidgetItem(QString::number(get<3>(fuell[i]))));
    }
    TableFuel->resizeColumnsToContents();//изменяем размер под слова

    for (unsigned int i = 0; i < 4; i++)
        TableFuel->item(0, i)->setBackground(QColor(0, 250, 154));//делаем первую строку зеленой

    for (unsigned int i = 0; i < 3; i++)//заносим данные для гистограмм
    {
        labels.append(QString::fromStdString(get<0>(fuell[i])));
        PercentData.append(get<1>(fuell[i]));
        ConsumptionData.append(get<2>(fuell[i]));
        MassEmissionsData.append(get<3>(fuell[i]));
    }

    Plot->clearPlottables();//очищаем полотно
    Bar();//заново создаем

    double Max = 0;//ищем максимальный процент
    for (double i:PercentData)
        if (Max < i)
            Max = i;

    Plot->yAxis->setRange(0, Max + 1);//от него строим высоту

    PercentCH->setPen(QPen(QColor(0, 191, 255).lighter(150)));//цвет гистограмм
    PercentCH->setBrush(QColor(0, 191, 255));
    Plot->yAxis->setLabel("Проценты, %");

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    Plot->xAxis->setTicker(textTicker);

    PercentCH->setData(ticks, PercentData);//создаем гистограмму
    Plot->replot();

    //при нажатии кнопки выводим все
    TableFuel->show();
    RadButtMass->show();
    RadButtPercent->show();
    RadButtRashod->show();
    Plot->show();
    pushCourier->show();
}

void WidgetFuelLocal::RadButtonChange()//меняем гистограммы в зависмости от кнопок
{
    Plot->clearPlottables();//очищаем и добавляем полотно
    Bar();
    if (RadButtPercent->isChecked())//если нажата кнопка процентов выводим гистограмму процентов
    {
        double Max = 0;
            for ( double i:PercentData)
                if (Max < i)
                    Max = i;

        PercentCH->setPen(QPen(QColor(0, 191, 255).lighter(150)));
        PercentCH->setBrush(QColor(0, 191, 255));
        Plot->yAxis->setLabel("Проценты, %");

        Plot->yAxis->setRange(0, Max + 1);
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(ticks, labels);
        Plot->xAxis->setTicker(textTicker);

        PercentCH->setData(ticks, PercentData);
    }
    if (RadButtRashod->isChecked())//иначе расхода
    {
        Consumption->setPen(QPen(QColor(0, 191, 255).lighter(150)));
        Consumption->setBrush(QColor(0, 191, 255));
        Plot->yAxis->setLabel("Масса, кг");

        Plot->yAxis->setRange(0, ConsumptionData[0] * 1.5);
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(ticks, labels);
        Plot->xAxis->setTicker(textTicker);

        Consumption->setData(ticks, ConsumptionData);
    }
    if (RadButtMass->isChecked())//иначе массы
    {
        Plot->yAxis->setRange(0, MassEmissionsData[0] * 1.5);
        MassEmissions->setPen(QPen(QColor(0, 191, 255).lighter(150)));
        MassEmissions->setBrush(QColor(0, 191, 255));

        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(ticks, labels);
        Plot->xAxis->setTicker(textTicker);

        Plot->yAxis->setLabel("Масса, кг");
        MassEmissions->setData(ticks, MassEmissionsData);
    }
    Plot->replot();
    Plot->show();
}

void WidgetFuelLocal::courier()//отправлячем по общей памяти данные
{
    //отправляем название топлив
    memName.setKey("KeyForName");

    if (memName.isAttached())
        if (!memName.detach())
            MessageBox->setText("Невозможно отделиться от общей памяти.");

    QBuffer bufferName;
    bufferName.open(QBuffer::ReadWrite);
    QDataStream outName(&bufferName);
    outName << labels;

    int sizeName = bufferName.size();

    if (!memName.create(sizeName))
    {
        MessageBox->setText(tr("Невозможно создать сегмент общей памяти."));
        return;
    }

    memName.lock();
    char *toName = static_cast<char*>(memName.data());
    const char *fromName = bufferName.data().data();
    memcpy(toName, fromName, qMin(memName.size(), sizeName));
    memName.unlock();
//------------------------------------------------
    //отправляем расход топлив
    memRashod.setKey("KeyForRashod");

    if (memRashod.isAttached())
        if (!memRashod.detach())
            MessageBox->setText("Невозможно отделиться от общей памяти.");

    QBuffer bufferRashod;
    bufferRashod.open(QBuffer::ReadWrite);
    QDataStream outRashod(&bufferRashod);
    outRashod << ConsumptionData;

    int sizeRashod = bufferRashod.size();

    if (!memRashod.create(sizeRashod))
    {
        MessageBox->setText(tr("Невозможно создать сегмент общей памяти."));
        return;
    }

    memRashod.lock();
    char *toRashod = static_cast<char*>(memRashod.data());
    const char *fromRashod = bufferRashod.data().data();
    memcpy(toRashod, fromRashod, qMin(memRashod.size(), sizeRashod));
    memRashod.unlock();
}

WidgetFuelLocal::WidgetFuelLocal(QWidget *parent)
{
    Q_UNUSED(parent)

    DatabaseFuel();//берем из базы данных топливо и СУЛА
    DatabasePPA();

    ticks << 1 << 2 << 3;
    ComboBoxPPA = new QComboBox;
    unsigned int nppa = static_cast<unsigned int>(ppa.size());
    for(unsigned int i = 0; i < nppa; i++)
        ComboBoxPPA->addItem(QString::fromStdString(get<0>(ppa[i])));//добавляем в комбобох СУЛА

    //объявляем виджеты
    QLabel* label = new QLabel("Длина, км - ");
    Distance = new QLineEdit("1");
    push = new QPushButton("Ok.");
    pushCourier = new QPushButton("Отправить данные");
    Plot = new QCustomPlot;
    RadButtPercent = new QRadioButton("Процентное содержание угл-ов");
    RadButtRashod = new QRadioButton("Расход топлива");
    RadButtMass = new QRadioButton("Масса выбросов");
    MessageBox = new QMessageBox;
    TableFuel = new QTableWidget(3, 4);

    //по дефолту стоит радиобатон с процентами
    RadButtPercent->setChecked(true);
    //куча косметики
    pushCourier->setStyleSheet("QPushButton {" "border-radius: 10px; padding: 6px; background-color: rgb(70, 130, 180);" "color: rgb(255, 255, 255);}"
                               "QPushButton:hover {" "background-color: rgb(65, 105, 225);}"
                               "QPushButton:pressed {" "background-color: rgb(0, 0, 128); } ");
    push->setCursor(Qt::PointingHandCursor);
    pushCourier->setCursor(Qt::PointingHandCursor);
    RadButtMass->setCursor(Qt::PointingHandCursor);
    RadButtPercent->setCursor(Qt::PointingHandCursor);
    RadButtRashod->setCursor(Qt::PointingHandCursor);

    //коннектим кнопки с функциями
    connect (push, &QPushButton::clicked, this, &WidgetFuelLocal::buttonclick);
    connect (RadButtPercent, &QRadioButton::clicked, this, &WidgetFuelLocal::RadButtonChange);
    connect (RadButtRashod, &QRadioButton::clicked, this, &WidgetFuelLocal::RadButtonChange);
    connect (RadButtMass, &QRadioButton::clicked, this, &WidgetFuelLocal::RadButtonChange);
    connect (pushCourier, &QPushButton::clicked, this, &WidgetFuelLocal::courier);

    //скрываем ряд виджетов
    RadButtMass->hide();
    RadButtPercent->hide();
    RadButtRashod->hide();
    TableFuel->hide();
    Plot->hide();
    pushCourier->hide();

    //создаем макеты и добавляем в них виджеты в определнном порядке
    QHBoxLayout* qhb = new QHBoxLayout;
    qhb->addWidget(ComboBoxPPA);
    qhb->addWidget(label);
    qhb->addWidget(Distance);

    QHBoxLayout* qhbRadButt = new QHBoxLayout;
    qhbRadButt->addWidget(RadButtPercent);
    qhbRadButt->addWidget(RadButtRashod);
    qhbRadButt->addWidget(RadButtMass);

    QVBoxLayout *qvb = new QVBoxLayout;
    qvb->addLayout(qhb);
    qvb->addWidget(push);
    qvb->addWidget(TableFuel, 0, Qt::AlignHCenter);
    qvb->addLayout(qhbRadButt);
    qvb->addWidget(Plot);
    qvb->addWidget(pushCourier);    

    setLayout(qvb);//выводим основной макет
}












