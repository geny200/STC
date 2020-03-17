import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.14

Drawer {
    id: id_drawer

    width: id_app.width / 4 > 200 ? id_app.width / 4 : 200
    height: id_app.height

    edge: Qt.RightEdge
    modal: true
    dim: true
    visible: true

    ToolBar {
        id: id_navigationRow

        width: parent.width

        Label {
            text: qsTr("Json Editor menu")

            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: id_back.left

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        ToolButton {
            id: id_back

            text: qsTr("→")
            font.pointSize: 16
            flat: true

            width: parent.width / 5
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right

            action: Action {
                shortcut: StandardKey.Back
            }

            onClicked: {
                if (id_stackView.depth <= 1)
                    id_drawer.visible = false
                else
                    id_stackView.pop()
            }
        }
    }

    Label {
        id: id_created

        text: qsTr("Created by Geny200")

        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Component {
        id: id_fileMenu

        ListView {
            delegate: ToolButton {
                text: model.text
                flat: true

                width: id_stackView.width

                onClicked: func()
            }

            model: ListModel {
                ListElement {
                    text: qsTr("&New")
                    func: () => createNewFile()
                }

                ListElement {
                    text: qsTr("&Open")
                    func: () => openFile()
                }

                ListElement {
                    text: qsTr("&Save")
                    func: () => saveFile()
                }

                ListElement {
                    text: qsTr("Save &As")
                    func: () => saveFileAs()
                }
            }
        }
    }
    Component {
        id: id_editMenu

        ListView {
            delegate: ToolButton {
                text: model.text
                flat: true

                width: id_stackView.width

                onClicked: func()
            }

            model: ListModel {
                ListElement {
                    text: qsTr("&Cut")
                    func: () => s_cut()
                }

                ListElement {
                    text: qsTr("&Copy")
                    func: () => s_copy()
                }

                ListElement {
                    text: qsTr("&Paste")
                    func: () => s_paste()
                }

                ListElement {
                    text: qsTr("&Delete")
                    func: () => s_delete()
                }

                ListElement {
                    text: qsTr("&Insert")
                    func: () => s_insert()
                }
            }
        }
    }

    ListView {
        id: id_settings

        visible: false

        delegate: Switch {
            id: id_switch
            text: state_f(Universal, id_switch)


            width: id_stackView.width

            onClicked: func()
        }

        model: ListModel {
            id: id_setModel

            ListElement {
                state_f: () => qsTr("Switch menu style")
                func: () => s_switchMenu()
            }

            ListElement {
                state_f: (Style) => Style.theme !== Style.Light ? qsTr("Light mode") : qsTr("Darck mode")
                func: () => s_switchMode()
            }
        }
    }

    StackView {
        id: id_stackView

        anchors.top: id_navigationRow.bottom
        anchors.right: parent.right
        anchors.bottom: id_created.top
        anchors.left: parent.left

        spacing: 1
        clip: true

        initialItem: ListView {
            delegate: ToolButton {
                text: model.text
                flat: true

                width: id_stackView.width

                onClicked: func()
            }

            model: ListModel {
                ListElement {
                    text: qsTr("&File")
                    func: () => id_stackView.push(id_fileMenu)
                }

                ListElement {
                    text: qsTr("&Edit")
                    func: () => id_stackView.push(id_editMenu)
                }

                ListElement {
                    text: qsTr("&Settings")
                    func: () => {
                              id_settings.visible = true
                              id_stackView.push(id_settings)
                          }
                }

                ListElement {
                    text: qsTr("&About")
                    func: () => id_dialogAbout.visible = true
                }

                ListElement {
                    text: qsTr("&Quit")
                    func: () => Qt.quit()
                }
            }
        }

        pushEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 0
                to:1
                duration: 200
            }
        }

        pushExit: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 1
                to:0
                duration: 200
            }
        }

        popEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 0
                to:1
                duration: 200
            }
        }

        popExit: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 1
                to:0
                duration: 200
            }
        }
    }
}

/*##^##
Designer {
    D{i:1;anchors_height:200;anchors_width:200}D{i:3;anchors_width:640}
}
##^##*/
