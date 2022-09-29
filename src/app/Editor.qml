import QtQuick
import QtQuick.Controls

ScrollView {
    property int indentation: 4
    property alias text: textArea.text
    property alias color: textArea.color
    property alias textDocument: textArea.textDocument

    ScrollBar.vertical.policy: ScrollBar.AlwaysOn

    TextArea {
        id: textArea

        function firstIndexNotMatching(arr, value, startIndex) {
            for (let i = startIndex; i < arr.length; ++i) {
                if (arr[i] != value)
                    return i;

            }
            return arr.length;
        }

        Keys.onPressed: (event) => {
            const start = textArea.selectionStart;
            const end = textArea.selectionEnd;
            if (event.key == Qt.Key_Tab) {
                if (start == end) {
                    const line_start = text.lastIndexOf('\n', start) + 1;
                    insert(start, ' '.repeat(indentation - (start - line_start) % indentation));
                } else if (!text.slice(start, end).includes('\n')) {
                    const line_start = text.lastIndexOf('\n', start) + 1;
                    remove(start, end);
                    insert(start, ' '.repeat(indentation - (start - line_start) % indentation));
                } else {
                    let current = end;
                    while (current > start) {
                        const line_start = text.lastIndexOf('\n', current) + 1;
                        insert(line_start, ' '.repeat(indentation));
                        current = line_start - 2;
                    }
                }
                event.accepted = true;
            }
            if (event.key == Qt.Key_Backtab) {
                let current = end;
                while (current >= start) {
                    const line_start = text.lastIndexOf('\n', current) + 1;
                    remove(line_start, Math.min(firstIndexNotMatching(text, ' ', line_start), line_start + indentation));
                    current = line_start - 2;
                }
                event.accepted = true;
            }
            if (event.key == Qt.Key_Enter || event.key == Qt.Key_Return) {
                const line_start = text.lastIndexOf('\n', start) + 1;
                const line_end = firstIndexNotMatching(text, ' ', line_start);
                remove(start, end);
                insert(start, '\n');
                insert(start + 1, ' '.repeat(line_end - line_start));
                event.accepted = true;
            }
        }
        wrapMode: TextEdit.Wrap
        selectByKeyboard: true
        selectByMouse: true
    }

}
