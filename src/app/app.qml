import Qt.labs.settings
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

    SpanHighlighter {
        id: spanHighlighter

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

            property bool blockedDataChanged: false

            Layout.fillWidth: true
            Layout.fillHeight: true
            SplitView.minimumHeight: 50
            color: "blue"
            font: fixedFont
            text: "0x1234'5678"
            onTextChanged: {
                if (!dataEdit.blockedDataChanged)
                    parser.dataChanged(text);

            }

            Connections {
                function onDataUsedChanged(start, end, spanType) {
                    if (!dataEdit.blockedDataChanged) {
                        dataEdit.blockedDataChanged = true;
                        spanHighlighter.spanChanged(start, end, spanType);
                    }
                    dataEdit.blockedDataChanged = false;
                }

                target: parser
            }

        }

    }

    Settings {
        property alias dataField: dataEdit.text
        property alias layoutField: layoutEdit.text
        property alias windowX: root.x
        property alias windowY: root.y
        property alias windowWidth: root.width
        property alias windowHeight: root.height
    }

}
