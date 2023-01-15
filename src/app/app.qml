import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import extrico 1.0

Window {
    id: root

    visible: true
    minimumWidth: 400
    minimumHeight: 200

    LayoutHighlighter {
        id: layoutHighlighter

        textDocument: layoutEdit.textDocument
    }

    HexHighlighter {
        id: hexHighlighter

        textDocument: dataEdit.textDocument
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical

        Parser {
            id: parser
        }

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
                text: "layout foo {\n  bar: i32;\n};"
                color: "green"
                onTextChanged: parser.layoutChanged(text)
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
                text: ""

                Connections {
                    function onValueChange(text) {
                        contentEdit.text = text;
                    }

                    target: parser
                }

            }

        }

        Editor {
            id: dataEdit

            Layout.fillWidth: true
            Layout.fillHeight: true
            SplitView.minimumHeight: 50
            color: "blue"
            font: fixedFont
            text: "0x1234'5678"
            onTextChanged: parser.dataChanged(text)
        }

    }

}
