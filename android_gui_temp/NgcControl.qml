import QtQuick 2.0
import QtQuick.Controls 2.12

Item {
    ListView{
        id: ngcListView
        anchors.fill: parent
        clip: true
        spacing: 80

        model: ngc_table

        /*Connections{
            target: orders_table

            onModelUpdated: {
                ordersListView.contentY = ordersListView.savedY;
            }
        }*/

        delegate: Row {
            Text{
                text: name + " | " + material + " | " + mapType + " | " + action
            }
            Button {
                text: "Запуск"
                //onClicked:
            }
        }

    }

}
