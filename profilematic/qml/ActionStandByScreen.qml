/**********************************************************************
 * Copyright 2011-2012 Ferdinand Mayet, Arto Jalkanen
 *
 * This file is part of ProfileMatic.
 *
 * ProfileMatic is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ProfileMatic is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ProfileMatic.  If not, see <http://www.gnu.org/licenses/>
**/
import QtQuick 1.1
import com.nokia.meego 1.0
import Rule 1.0
import "UIConstants.js" as UIConstants

Page {
    id: root
    tools: commonTools
    anchors.margins: UIConstants.DEFAULT_MARGIN

    property RuleAction action;

    Flickable {
        anchors.fill: parent
        anchors.topMargin: header.height
        pressDelay: 140
        clip: true
        contentWidth: parent.width
        contentHeight: container.height // contentItem.childrenRect.height

        Column {
            id: container
            spacing: UIConstants.PADDING_XXLARGE
            anchors.verticalCenter: parent.verticalCenter

            width: parent.width
            height: childrenRect.height

            PageHeader {
                text: "Stand-by screen action"
            }

            RuleTopicSummary {
                id: standByScreen
                topic: "Select stand-by screen mode"
                summary: standByScreenModeSummary();
                showComboBox: true
                onTopicClicked: standByScreenModeEditHandler()
            }

            Item {
                id: restoreContainer
                width: parent.width
                height: action.standByScreenMode >= 0 ? restore.height : 0
                clip: true

                Behavior on height {
                    NumberAnimation { duration: 300 }
                }
                RuleTopicSummary {
                    id: restore
                    topic: "Restore previous stand-by screen mode"
                    topicHeight: Math.max(topicImplicitHeight, restoreSwitch.height)
                    topicWidth: parent.width - restoreSwitch.width
                    summary: restoreSwitch.checked ? "The previous stand-by screen mode will be restored."
                                                   : "Previous stand-by screen mode will not be restored."
                    onTopicClicked: restoreSwitch.checked = !restoreSwitch.checked
                }
                Switch {
                    id: restoreSwitch
                    checked: action.restoreStandByScreenMode
                    anchors.right: restoreContainer.right
                    anchors.top: parent.top
                    anchors.verticalCenter: parent.top
                    onCheckedChanged: {
                        action.restoreStandByScreenMode = checked
                    }
                }
            }
        }
    }

    StandByScreenModeDialog {
        id: dStandByScreenMode

        onStandByScreenModeSelected: {
            action.standByScreenMode = selectedStandByScreenMode
        }
    }

    function standByScreenModeSummary() {
        return dStandByScreenMode.standByScreenModeToText(action.standByScreenMode)
    }

    function standByScreenModeEditHandler() {
        dStandByScreenMode.selectedStandByScreenMode = action.standByScreenMode
        dStandByScreenMode.open();
    }
}
