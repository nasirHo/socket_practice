import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Chat Server")

    Connections {
            target: server
            function onNewMessage(type, name, ba) {
                listModelMessages.append({
                                             message: name + "\n" + ba
                                         })
                listViewMessages.positionViewAtEnd()
            }
    }


    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        ListView {
            id: listViewMessages
            Layout.fillHeight: true
            Layout.fillWidth: true
            clip: true
            model: ListModel {
                id: listModelMessages
                ListElement {
                    message: "Welcome to chat server"
                }
            }
            delegate: ItemDelegate {
                text: message
            }
            ScrollBar.vertical: ScrollBar {}
        }
        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: textFieldMessage
                placeholderText: qsTr("Your message ...")
                Layout.fillWidth: true
                onAccepted: buttonSend.clicked()
            }
            Button {
                id: buttonSend
                text: qsTr("Send")
                onClicked: {
                    server.sendMessage(textFieldMessage.text)
                    textFieldMessage.clear()
                }
            }
        }
    }
}
