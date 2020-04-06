import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.14
import QtQuick.Dialogs 1.3 as Dialog1

ApplicationWindow {
    id: id_app

    //announcement of signals
    signal s_saveFile(string fileName)
    signal s_loadFile(string fileName)
    signal s_copy()
    signal s_paste()
    signal s_cut()
    signal s_create()
    signal s_insert()
    signal s_delete()
    signal s_switchMenu()
    signal s_switchMode()

    width: 640
    height: 480

    visible: true
    title: qsTr("Json Editor")

    header: ToolBar {       //auxiliary element for the draw menu
        id: id_toolBar

        ToolButton {
            text: qsTr("≡")
            font.pointSize: 16

            anchors.right: parent.right

            onClicked: id_drawer.visible = true
        }
    }

    menuBar: GMenuBar {     //standard menubar
        id: id_menuBar;

        enabled: false;
        visible: false
    }

    GMenu {                 //draw menu
        id: id_drawer
    }

    GJsonTree {             //tree view
        id: id_view
    }

    //menu dialogs
    Dialog1.FileDialog {
        id: id_dialogFile

        property var callBack: null

        nameFilters:  [
            qsTr("Json files")  + " (*.json)",
            qsTr("All files")   + " (*)",
        ]
        folder: shortcuts.documents

        visible: false
        selectExisting: true

        onAccepted: callBack(id_dialogFile.fileUrl.toString().replace("file:///",""))
        onRejected: visible = false
    }

    Dialog {
        id: id_dialogAbout

        title: qsTr("About")
        visible: false
        modal: true

        anchors.centerIn: Overlay.overlay
        standardButtons: Dialog.Ok

        Label {
            text: qsTr("Json Editor created by Geny200")
        }
    }

    Dialog {
        id: id_dialogHelp

        title: qsTr("Help")
        visible: false
        modal: true
        clip: true

        anchors.centerIn: Overlay.overlay
        standardButtons: Dialog.Ok
        height: parent.height < 350 ? 350 : (parent.height > 700 ? 700 : parent.height)
        width: 400


        Label {
            id: id_helpOne

            width: parent.width
            wrapMode: "WordWrap"

            horizontalAlignment: "AlignLeft"
            text: qsTr("- To select, click on the desired field once.")
        }

        Label {
            id: id_helpTwo

            anchors.topMargin: 5
            anchors.top: id_helpOne.bottom
            width: parent.width
            wrapMode: "WordWrap"

            horizontalAlignment: "AlignLeft"
            text: qsTr("- To change the data, double-click on the desired field. To save the changed data in the field press \"Enter\". To cancel press \"Esc\" or change focus to another element")
        }

        Label {
            id: id_helpThree

            anchors.topMargin: 5
            anchors.top: id_helpTwo.bottom
            width: parent.width
            wrapMode: "WordWrap"

            horizontalAlignment: "AlignLeft"
            text: qsTr("- To change the structure use the menu or shortcut.")
        }

        Label {
            id: id_helpFour

            anchors.topMargin: 5
            anchors.top: id_helpThree.bottom
            width: parent.width
            wrapMode: "WordWrap"

            horizontalAlignment: "AlignLeft"
            text: qsTr("- List of available shortcut:")
        }

        ListView {
            ScrollBar.vertical: ScrollBar {
              visible: true
            }
            clip: true
            id: id_listHelp
            anchors.top: id_helpFour.bottom
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            delegate: ToolButton {
                text:  model.text

                flat: true
                width: id_listHelp.width
            }

            model: ListModel {
                ListElement {
                    text: qsTr("ctrl + C - Copy selected item")
                }
                ListElement {
                    text: qsTr("ctrl + V - Paste after selected item")
                }
                ListElement {
                    text: qsTr("ctrl + X - Cut selected item")
                }
                ListElement {
                    text: qsTr("ctrl + N - Create a new item after the selected")
                }
                ListElement {
                    text: qsTr("delete - Delete selected item")
                }
                ListElement {
                    text: qsTr("ctrl + S - Save file")
                }
                ListElement {
                    text: qsTr("ctrl + S + A - Save as")
                }
                ListElement {
                    text: qsTr("ctrl + O - Open file")
                }
            }
        }

    }

    Dialog {
        id: id_dialogFileNotSaved

        title: qsTr("Attention")
        visible: false
        modal: true

        anchors.centerIn: Overlay.overlay
        standardButtons: Dialog.Ok

        Label {
            text: qsTr("File didn't saved")
        }
    }

    Dialog {
        id: id_dialogMessage

        property var callBack: null

        title: qsTr("Attention")
        visible: false
        modal: true

        anchors.centerIn: Overlay.overlay
        standardButtons: Dialog.Yes | Dialog.No

        Label {
            text: qsTr("Close current file?")
        }
        onAccepted: callBack()
    }

    //hotkeys
    Shortcut {
        sequence: StandardKey.Cut
        onActivated: s_cut()
    }

    Shortcut {
        sequence: StandardKey.Copy
        onActivated: s_copy()
    }

    Shortcut {
        sequence: StandardKey.Paste
        onActivated: s_paste()
    }

    Shortcut {
        sequence: StandardKey.Delete
        onActivated: s_delete()
    }

    Shortcut {
        sequence: StandardKey.New
        onActivated: s_create()
    }

    Shortcut {
        sequence: StandardKey.Open
        onActivated: openFile()
    }

    Shortcut {
        sequence: StandardKey.Save
        onActivated: saveFile()
    }

    Shortcut {
        sequence: StandardKey.SaveAs
        onActivated: saveFileAs()
    }

    Shortcut {
        sequence: StandardKey.Quit
        onActivated: Qt.quit()
    }

    //basic function
    function openFileDialog(titleName, callBack) {
        id_dialogFile.title = titleName;
        id_dialogFile.callBack = callBack;
        id_dialogFile.visible = true;
    }

    function openFile() {
        id_dialogFile.selectExisting = true
        openFileDialog(qsTr("Open file"), (path) => {
                           s_loadFile(path)
                       })
    }

    function saveFileAs() {
        id_dialogFile.selectExisting = false
        openFileDialog(qsTr("Save"), (path) => {
                           s_saveFile(path)
                           if (!jsonEditor.fileSaved)
                           id_dialogFileNotSaved.visible = true
                       })
    }

    function saveFile() {
        if (jsonEditor.fileSaved)
            s_saveFile("")
        else
            saveFileAs()
    }

    function createNewFile() {
        id_dialogMessage.callBack = () => {
            jsonModel.clear()
            jsonModel.create()
            jsonEditor.fileSaved = false
        }
        id_dialogMessage.visible = true
    }

    //signals realization
    onS_switchMenu: {
        id_toolBar.enabled = !id_toolBar.enabled
        id_toolBar.visible = !id_toolBar.visible

        id_menuBar.enabled = !id_menuBar.enabled
        id_menuBar.visible = !id_menuBar.visible

        id_drawer.enabled = !id_drawer.enabled
        id_drawer.visible = !id_drawer.visible
    }

    onS_switchMode: {
        if (Universal.theme == Universal.Light)
            Universal.theme = Universal.Dark
        else
            Universal.theme = Universal.Light
    }

    onS_copy: id_view.s_copy()
    onS_paste: id_view.s_paste()
    onS_cut: id_view.s_cut()
    onS_create: id_view.s_create()
    onS_delete: id_view.s_delete()
    onS_insert: id_view.s_insert()
    onS_saveFile: jsonEditor.sl_saveFile(fileName)
    onS_loadFile: jsonEditor.sl_loadFile(fileName)
}


