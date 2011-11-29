import QtQuick 1.1
import BattleCity 1.0

Rectangle {
    color: "#909090"

    width: mapEditor.width
    height: mapEditor.height

    BCMapEditor {
        id: mapEditor
        anchors.centerIn: parent
    }
}
