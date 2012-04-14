/**********************************************************************
 * Copyright 2012 Arto Jalkanen
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

    property RuleCondition condition;
    property int maxCells: 50

    Flickable {
        anchors.fill: parent
        // anchors.topMargin: header.height
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

            TextFieldWithLabel {
                labelText: "Idle in seconds"
                placeholderText: "Not set"
                text: condition.idleForSecs < 0 ? "" : condition.idleForSecs
                inputMethodHints: Qt.ImhDigitsOnly
                inputMask: "00000"
                width: parent.width
                onTextChanged: {
                    if (root.status === PageStatus.Active) {
                        condition.idleForSecs = (text !== "" ? parseInt(text) : -1)
                    }
                }
            }

            Label {
                id: help
                text: "Idle means that the device has been in stand-by screen mode or lock screen mode at "
                    + "least for the amount of seconds specified."
                width: parent.width
                platformStyle: LabelStyleSubtitle {
                    fontPixelSize: UIConstants.FONT_SMALL
                    textColor: !theme.inverted ? UIConstants.COLOR_SECONDARY_FOREGROUND : UIConstants.COLOR_INVERTED_SECONDARY_FOREGROUND
                }
            }

        } // Column
    } // Flickable
}