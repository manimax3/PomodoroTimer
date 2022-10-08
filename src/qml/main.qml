import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Backend

Window {
	visible: true
	width: 420
	height: 420
	maximumWidth: 420
	maximumHeight: 420
	minimumWidth: 420
	minimumHeight: 420
	title: "Pomodoro Timer"
	color: "#222222"

	PomoManager {
		id: pomoManager
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 4

		Text {
			Layout.alignment: Qt.AlignCenter
			text: pomoManager.state
			font.pointSize: 28
			color: "white"
		}

		Text {
			Layout.alignment: Qt.AlignCenter
			text: pomoManager.timeRemaining
			font.pointSize: 42
			color: "white"
		}

		RowLayout {
			id: indicators
			Layout.alignment: Qt.AlignCenter
			property int indicatorSize: 18
			spacing: indicatorSize

			Repeater {
				model: pomoManager.countPomodoros
				Rectangle {
					color: model.index == pomoManager.activePomoIndex ? "white" : "gray"
					width: parent.indicatorSize
					height: width
					radius: width / 2

					MouseArea {
						anchors.fill: parent
						onClicked: {
							if (pomoManager.activePomoIndex  != model.index) {
								pomoManager.activePomoIndex = model.index
							}
						}
					}
				}
			}

		}

		RowLayout {
			Layout.alignment: Qt.AlignCenter

			ListModel {
				id: actionModel
				ListElement {
					text: "Start"
				}

				ListElement {
					text: "Pause"
				}

				ListElement {
					text: "Stop"
				}

				ListElement {
					text: "Settings"
				}
			}

			Repeater {
				model: actionModel

				Rectangle {
					width: 80
					height: 48
					color: "#555555"
					radius: 4

					Text {
						text: model.text
						color: "white"
						font.pointSize: 14
						anchors.centerIn: parent
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							switch(model.index) {
								case 0:
									pomoManager.start();
									break;
								case 1:
									pomoManager.pause();
									break;
								case 2:
									pomoManager.stop();
									break;
								case 3:
									pomoManager.viewSettingsFolder();
									break;
							}
						}
					}
				}
			}
		}
	}

}
