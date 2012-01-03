import QtQuick 1.1
import BattleCity 1.0

Rectangle {
    color: "#909090"

    width: board.width + 2 * board.cellSize + mainLayout.spacing + 2 * mainLayout.anchors.margins + mainLayout.spacing
    height: board.height + board.cellSize + mainLayout.spacing + 4 * mainLayout.anchors.margins

    BCMapsManager {
        id: mapsManager

        Component.onCompleted: mapsManager.loadMap(mapsManager.maps[0], board)

        onMapLoaded: internal.init()
    }

    Component.onCompleted: internal.init()

    Row {
        id: mainLayout

        anchors.fill: parent
        anchors.margins: 5

        spacing: 5

        Column {
            spacing: 5

            BCBoard {
                id: board

                MouseArea {
                    id: mouseArea
                    z: -1
                    anchors.fill: parent
                    hoverEnabled: true

                    onPositionChanged: {
                        var cell = board.childAt(mouse.x, mouse.y);
                        if (!cell)
                            return;
                        boardCursor.x = cell.x + mainLayout.anchors.margins;
                        boardCursor.y = cell.y + mainLayout.anchors.margins;

                        if (mouseArea.pressed)
                            board.setObstacleType(cell.row, cell.column, internal.currentObstacle);
                    }
                    onPressed: {
                        var cell = board.childAt(mouse.x, mouse.y);
                        if (!cell)
                            return;
                        board.setObstacleType(cell.row, cell.column, internal.currentObstacle);
                    }
                }
            }

            Row {
                id: buttonsLayout
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 5

                Row {
                    spacing: 5
                    Repeater {
                        model: obstacleModel
                        delegate: Item {
                            id: button
                            width: childrenRect.width
                            height: childrenRect.height

                            Grid {
                                rows: 2
                                columns: 2

                                Repeater {
                                    model: 4
                                    Pixmap {
                                        pixmap: BattleCityInstance.obstacleTexture(obstacle)
                                        width: board.obsticaleSize
                                        height: board.obsticaleSize
                                    }
                                }
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    internal.currentObstacle = obstacle;
                                    board.setCursor(obstacle);
                                    buttonCursor.x = buttonsLayout.x + button.x
                                    buttonCursor.y = buttonsLayout.y + button.y
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    color: "black"

                    width: 80
                    height: 2 * board.obsticaleSize

                    Text {
                        id: showGridTitle
                        anchors.centerIn: parent
                        text: board.gridVisible ? qsTr("Hide grid") : qsTr("Show grid")
                        color: "white"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: board.gridVisible = !board.gridVisible
                    }
                }

                Rectangle {
                    color: "black"

                    width: 80
                    height: 2 * board.obsticaleSize

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Save")
                        color: "white"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: mapsManager.saveMap(board)
                    }
                }
            }
        }

        Grid {
            anchors.verticalCenter: parent.verticalCenter

            rows: 10
            columns: 2
            spacing: 5

            Repeater {
                model: tanksModel
                delegate: Item {
                    id: tank
                    width: childrenRect.width
                    height: childrenRect.height

                    Pixmap {
                        id: image
                        pixmap: BattleCityInstance.tankTexture(type, BattleCity.Forward, 0, bonus)
                        width: board.cellSize
                        height: board.cellSize
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            var nextIndex = internal.nextIndex(type);
                            var tempBonus = bonus;
                            if (!nextIndex)
                                tempBonus = !bonus;
                            var tankType = internal.tanks[nextIndex];
                            tanksModel.set(index, {"bonus": tempBonus, "type": tankType, "index": index});
                            board.setEnemyTankType(index, tankType, tempBonus);
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: boardCursor

        border.width: 1
        border.color: "white"

        opacity: 0.25

        x: board.x + mainLayout.anchors.margins
        y: board.y + mainLayout.anchors.margins
        width: board.obsticaleSize
        height: board.obsticaleSize
    }

    Rectangle {
        id: buttonCursor

        border.width: 4
        border.color: "white"

        color: "transparent"

        x: buttonsLayout.x
        y: buttonsLayout.y
        width: board.cellSize + 2.5 * border.width
        height: board.cellSize + 2.5 * border.width
    }

    ListModel {
        id: obstacleModel
    }

    ListModel {
        id: tanksModel
    }

    QtObject {
        id: internal

        property int currentObstacle: BattleCity.Ground
        property variant tanks: [BattleCity.Basic, BattleCity.Fast, BattleCity.Power, BattleCity.Armor]

        function init()
        {
            obstacleModel.clear();
            obstacleModel.append({"obstacle": BattleCity.Ground});
            obstacleModel.append({"obstacle": BattleCity.BricksWall});
            obstacleModel.append({"obstacle": BattleCity.ConcreteWall});
            obstacleModel.append({"obstacle": BattleCity.Ice});
            obstacleModel.append({"obstacle": BattleCity.Camouflage});
            obstacleModel.append({"obstacle": BattleCity.Water});

            tanksModel.clear();
            for (var index = 0; index < board.enemyTanksCount; ++index) {
                var tank = board.enemyTank(index);
                tanksModel.append({"bonus": tank.bonus, "type": tank.type, "index": index});
            }
        }

        function nextIndex(tankType)
        {
            var index = tankTypeToIndex(tankType) + 1;
            return index === internal.tanks.length ? 0 : index;
        }

        function tankTypeToIndex(tankType)
        {
            var res = -1;
            switch (tankType) {
            case BattleCity.Basic:
                res = 0;
                break;
            case BattleCity.Fast:
                res = 1;
                break;
            case BattleCity.Power:
                res = 2;
                break;
            case BattleCity.Armor:
                res = 3;
                break;
            }
            return res;
        }
    }
}
