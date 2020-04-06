import QtQuick 2.14
import QtQuick.Controls 1.5 as QuickControl1
import QtQuick.Controls 2.14
import QtQml.Models 2.2
import QtQuick.Controls.Universal 2.14

QuickControl1.TreeView {
    id: id_view

    //announcement of signals
    signal s_copy()
    signal s_paste()
    signal s_cut()
    signal s_delete()
    signal s_create()
    signal s_insert()

    anchors.fill: parent
    model: jsonModel
    backgroundVisible: false
    clip: true

    headerDelegate: Rectangle {
        height: 40

        color: Universal.background

        Frame {
            anchors.fill: parent
            Label {
                text: styleData.value
                elide: "ElideRight"

                anchors.fill: parent

                horizontalAlignment: styleData.textAlignment
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    QuickControl1.TableViewColumn {
        title: "Name"
        role: "name"

        resizable: true
        movable: false
        horizontalAlignment: Text.AlignLeft
    }

    QuickControl1.TableViewColumn {
        title: "Id"
        role: "id"

        resizable: true
        movable: false
        horizontalAlignment: Text.AlignRight
    }

    selection: ItemSelectionModel {
        id: id_sel
        model: jsonModel
    }

    rowDelegate: Rectangle {
        id: id_row

        height: 40

        state: styleData.selected ? "selected" : "normal"
        focus: true

        onFocusChanged: {
            if (state == "selected")
                id_row.forceActiveFocus()
        }

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            onPressed: {
                mouse.accepted = false
            }
        }

        states: [
            State {
                name: "normal"
                PropertyChanges { target: id_row; focus: false; color: styleData.alternate ? Universal.baseLowColor : Universal.background;}
            },
            State {
                name: "selected"
                PropertyChanges { target: id_row; focus: true; color: Universal.accent}
            }
        ]
    }

    itemDelegate: Control {
        id: id_item

        state: id_field.activeFocus ? "edit" :"static"

        Label{
            id: id_label

            text: styleData.value
            elide: styleData.elideMode
            color: (styleData.selected ? Universal.background : Universal.foreground);

            anchors.fill: parent

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: styleData.textAlignment
        }

        TextField {
            id: id_field

            text: styleData.value
            clip: true

            anchors.fill: parent

            horizontalAlignment: styleData.textAlignment

            onAccepted: {
                jsonModel.set_data(styleData.index, id_field.text, styleData.column)
                id_item.forceActiveFocus()
            }
            Keys.onEscapePressed: id_item.forceActiveFocus()
        }

        MouseArea {
            id: id_mouseArea

            anchors.fill: parent

            propagateComposedEvents: true

            onPressed:  {
                if (mouse.flags === Qt.MouseEventCreatedDoubleClick)
                    id_field.forceActiveFocus()
                mouse.accepted = false
            }
        }

        states: [
            State {
                name: "static"
                PropertyChanges { target: id_label;     visible: true; }
                PropertyChanges { target: id_mouseArea; visible: true; }
                PropertyChanges { target: id_field;     visible: false; focus: false; text: styleData.value; }
            },

            State {
                name: "edit"
                PropertyChanges { target: id_label;     visible: false; }
                PropertyChanges { target: id_mouseArea; visible: false; }
                PropertyChanges { target: id_field;     visible: true; focus: true; text: styleData.value; }
            }
        ]
    }

    //signals realization
    onS_delete: {
        let selectIndex = id_view.selection.selectedIndexes
        if (!selectIndex[0])
            return

        let parentIndex = selectIndex[0].parent
        let selectRow = selectIndex[0].row

        let newIndex = jsonModel.index(selectRow + 1, 0, parentIndex)
        if (!newIndex.valid && selectRow !== 0)
            newIndex = jsonModel.index(selectRow - 1, 0, parentIndex)

        if (!newIndex.valid)
            newIndex = parentIndex

        if (!newIndex.valid)
            id_sel.clear()
        else
            id_sel.setCurrentIndex(newIndex, ItemSelectionModel.Select | ItemSelectionModel.Current)

        jsonModel.removeRows(selectRow, 1, parentIndex)
    }

    onS_cut:  {
        jsonEditor.sl_copy(id_sel.currentIndex)
        s_delete()
    }

    onS_copy: jsonEditor.sl_copy(id_sel.currentIndex)
    onS_paste: jsonEditor.sl_paste(id_sel.currentIndex)
    onS_create: jsonEditor.sl_new(id_sel.currentIndex)
    onS_insert: jsonEditor.sl_insert(id_sel.currentIndex)
}
