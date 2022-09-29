import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import extrico 1.0

Window {
    id: root

    visible: true
    minimumWidth: 400
    minimumHeight: 200

    HexHighlighter {
        id: hexHighlighter

        textDocument: textEdit.textDocument
    }

    LayoutHighlighter {
        id: layoutHighlighter

        textDocument: layoutEdit.textDocument
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical

        SplitView {
            SplitView.minimumHeight: 50
            SplitView.preferredHeight: parent.height / 2
            orientation: Qt.Horizontal

            Editor {
                id: layoutEdit

                SplitView.preferredWidth: parent.width / 2
                SplitView.minimumWidth: 20
                Layout.minimumWidth: 50
                Layout.minimumHeight: 50
                Layout.fillWidth: true
                Layout.fillHeight: true
                font: fixedFont
                text: "Lorem ipsum dolor sit amet, consectetur adipisicing elit, " + "sed do eiusmod tempor incididunt ut labore et dolore magna " + "aliqua. Ut enim ad minim veniam, quis nostrud exercitation " + "ullamco laboris nisi ut aliquip ex ea commodo cosnsequat. "
                color: "green"
            }

            Editor {
                id: contentEdit

                SplitView.minimumWidth: 20
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 50
                Layout.minimumHeight: 50
                color: "red"
                font: fixedFont
                text: "Lorem ipsum dolor sit amet, consectetur adipisicing elit, " + "sed do eiusmod tempor incididunt ut labore et dolore magna " + "aliqua. Ut enim ad minim veniam, quis nostrud exercitation " + "ullamco laboris nisi ut aliquip ex ea commodo cosnsequat. "
            }

        }

        Editor {
            id: textEdit

            Layout.fillWidth: true
            Layout.fillHeight: true
            SplitView.minimumHeight: 50
            color: "blue"
            font: fixedFont
            text: "Lorem ipsum dolor sit amet, consectetur adipisicing elit, " + "sed do eiusmod tempor incididunt ut labore et dolore magna " + "aliqua. Ut enim ad minim veniam, quis nostrud exercitation " + "ullamco laboris nisi ut aliquip ex ea commodo cosnsequat. "
        }

    }

}
