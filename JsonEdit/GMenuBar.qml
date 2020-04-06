import QtQuick 2.14
import QtQuick.Controls 2.14

MenuBar {
    Menu {
        title: qsTr("&File")
        dim: true

        MenuItem {
            text: qsTr("&New...")
            onTriggered: createNewFile()
        }

        MenuItem {
            text: qsTr("&Open...")
            onTriggered: openFile()
        }

        MenuItem {
            text: qsTr("&Save")
            onTriggered: saveFile()
        }

        MenuItem {
            text: qsTr("Save &As...")
            onTriggered: saveFileAs()
        }

        MenuSeparator {}

        MenuItem {
            text: qsTr("&Quit")
            onTriggered: Qt.quit()
        }
    }

    Menu {
        title: qsTr("&Settings")
        dim: true

        MenuItem {
            text: qsTr("&Switch menu style")
            onTriggered: s_switchMenu()
        }
    }

    Menu {
        title: qsTr("&Edit")
        dim: true

        MenuItem {
            text: qsTr("Cu&t")
            onTriggered: s_cut()
        }

        MenuItem {
            text: qsTr("&Copy")
            onTriggered: s_copy()
        }

        MenuItem {
            text: qsTr("&Paste")
            onTriggered: s_paste()
        }

        MenuSeparator {}

        MenuItem {
            text: qsTr("&New")
            onTriggered: s_create()
        }

        MenuItem {
            text: qsTr("&Delete")
            onTriggered: s_delete()
        }

        MenuItem {
            text: qsTr("&Insert")
            onTriggered: s_insert()
        }
    }

    Menu {
        title: qsTr("&Help")
        dim: true

        MenuItem {
            text: qsTr("Sho&rtcut")
            onTriggered: id_dialogHelp.visible = true
        }

        MenuItem {
            text: qsTr("&About")
            onTriggered: id_dialogAbout.visible = true
        }
    }
}
