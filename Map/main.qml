import QtQuick 2.7
import QtLocation 5.11
import QtPositioning 5.11
import QtQuick.Window 2.10
import QtQuick.Dialogs 1.3

import MarkerModel 1.0//импортириуем тот самый класс из marker.h

Item
{
    //объявляем переменные
    property var coordinateStart
    property var coordinateEnd
    property double startX
    property double startY
    property double endX
    property double endY
    property bool param
    property variant textLabel: []
    property var typeMap
    property var travelMode
    property int i

    //ставим видимость и размеры на весь экран
    visible: true
    width: Screen.width
    height: Screen.height

    PositionSource//класс запрашивающий местонахождение
    {
        id: src
        updateInterval: 1000
    }

    //функции которые принимают переменные из map.cpp или же там вызываются
    function label (words)//в массив закидываем текст для меток
    {
        i = i + 1
        textLabel[i-1] = words
    }

    function search (city)//в деокодмодель присваиваем адресс и обновляем его дабы перейти по этому адресу
    {
        geocodeModel.query = city
        geocodeModel.update()
    }

    function home ()//при нажатии на кнопку "Домой" запрашиваем геолокацию и переносимся в место жительство, иначе выводи ошибку
    {
        src.active = true
        if (src.position.longitudeValid)
            return map.center = src.position.coordinate
        else
            return message.visible = true
    }

    function typeMaps (type)//тип карты
    {
        map.activeMapType = map.supportedMapTypes[type]
    }

    Plugin//импортируем карту openstreetmap в программу
    {
        id: plugin
        name: "osm"

        PluginParameter { name: "osm.mapping.host"; value: "http://a.tile.openstreetmap.org/" }
    }

    //принимаем координаты начала и конца маршрута
    Waypoint
    {
        id: startCoor
        coordinate: QtPositioning.coordinate(startX, startY)
    }

    Waypoint
    {
        id: endCoor
        coordinate: QtPositioning.coordinate(endX, endY)
    }

    RouteQuery//добавляем эти координаты в маршрут
    {
        id: query
        Component.onCompleted:
        {
            addWaypoint(startCoor);
            addWaypoint(endCoor);
        }
    }

    //модель маршрута
    RouteModel
    {
        id: routeModel
        objectName: "routeModel"
        plugin: plugin
        query: query

        //функции отвечают за то включена ли кнопка "маршрут" или нет
        function modelOff ()
        {
            param = false
            routeModel.reset()
            autoUpdate = false
        }

        function modelOn ()
        {
            param = true
            autoUpdate = true
        }
    }

    MarkerModel { id: markerModel }//объявление модели-класса

    Map//карта
    {
        id: map
        objectName: "map"
        anchors.fill: parent
        zoomLevel: 15
        plugin: plugin
        focus: true
        center: QtPositioning.coordinate(55.810767, 37.502876)
        activeMapType: supportedMapTypes[6]

        MapItemView//объект который показывает маршрут на карте
        {
            model: routeModel
            delegate: MapRoute
            {
                route: routeData
                line.color: "blue"
                line.width: 5
                smooth: false
            }
        }

        MapItemView//показывает на карте маркеры и лэйблы
        {
            model: markerModel
            delegate: componentMarker
        }

        MapItemView
        {
            model: markerModel
            delegate: componentLabel
        }

        GeocodeModel//для поиска на карте
        {
              id: geocodeModel
              plugin: plugin
              autoUpdate: false
              onLocationsChanged: { map.center = get(0).coordinate }
        }

        MouseArea
        {
            anchors.fill: parent
            objectName: "mouse"
            property int count: 0
            onClicked:
            {
                if (param)//если переключатель "маршрут" включен тособираем координаты первого клика(начало маршрута) и второго клика(конца маршрута)
                {
                    if (mouse.isClick)
                        count = count + 1
                    if (count === 1)
                    {
                        //обновляем маршрут
                        query.removeWaypoint(endCoor)
                        routeModel.reset()

                        //присваиваем координаты щелчка мыши
                        coordinateStart = map.toCoordinate(Qt.point(mouse.x, mouse.y))
                        startX = coordinateStart.latitude
                        startY = coordinateStart.longitude
                    }
                    if (count === 2)
                    {
                        coordinateEnd = map.toCoordinate(Qt.point(mouse.x, mouse.y))
                        endX = coordinateEnd.latitude
                        endY = coordinateEnd.longitude
                        query.addWaypoint(endCoor)
                        count = 0
                    }
                }
                else//если "маршрут" не включен то используем маркеры
                {
                    var coordinate = map.toCoordinate(Qt.point(mouse.x,mouse.y))
                    markerModel.addMarker(coordinate)//добавляем в модель координаты щелчка мыши
                }
            }
        }
    }

    MessageDialog//ошибка если не включена геолокация при нажатии на "Домой"
    {
        id: message
        title: "Ошибка:"
        text: "Пожалуйста, включите геолокацию для работы кнопки 'Домой'"
    }

    Component//компонент котрый добавляет метку
    {
        id: componentMarker
        MapQuickItem
        {
            id: marker
            anchorPoint.x: image.width/4
            anchorPoint.y: image.height
            coordinate: position

            sourceItem: Image
            {
                id: image
                source: "qrc:/metka.png"
            }
        }
    }

    Component//компонент который добавляет текст
    {
        id: componentLabel
        MapQuickItem
        {
            id:label
            anchorPoint.x: width/6
            anchorPoint.y: height*2+3
            coordinate: position
            clip: true
            sourceItem:
            Text
            {
                width: 200
                wrapMode: "WordWrap"
                text: textLabel[i-1]
            }
        }
    }
}
