import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    id:window
    visible: true
    width: 1350
    height: 400
    title: qsTr("Эффективность работы")

    Component.onCompleted: {
        if (!ngc_table.fetchNgc()) tryFetchButton.visible = true;

        showMaximized()
    }

    TabBar {
        id: bar
        width: parent.width
        TabButton {
            text: qsTr("Программы")
        }
        TabButton {
            text: qsTr("Консоль")
        }
    }

    Row{
        id: messageRow
        anchors.top: bar.bottom
        Text {
            id: stateMessage
            font.pointSize: 14
        }

        Button{
            id: tryFetchButton
            visible: false
            font.pointSize: 10
            text: "Попробовать снова"
            onClicked: {
                if (ngc_table.fetchNgc()) tryFetchButton.visible = false
            }
        }
        Connections{
            target: ngc_table
            onMessageChanged: stateMessage.text = msg
        }
    }

    SwipeView {
        interactive: false
        padding: 20
        width: parent.width
        anchors.top: messageRow.bottom
        anchors.bottom: parent.bottom

        currentIndex: bar.currentIndex

        NgcControl {
            id: ngc_control
        }
        Item {
            //id: console
        }
    }
}
