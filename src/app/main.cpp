#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickTextDocument>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

#include <iostream>

struct HighlightingRule {
    QRegularExpression pattern;
    QTextCharFormat format;
};

class HexHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument* textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)

public:
    HexHighlighter(QObject* parent = nullptr) : QSyntaxHighlighter(parent), m_TextDocument(nullptr) {}

signals:
    void textDocumentChanged();

protected:
    QQuickTextDocument* m_TextDocument;

    QQuickTextDocument* textDocument() const { return m_TextDocument; }
    void setTextDocument(QQuickTextDocument* textDocument) {
        if (textDocument == m_TextDocument) {
            return;
        }

        m_TextDocument = textDocument;

        QTextDocument* doc = m_TextDocument->textDocument();
        setDocument(doc);

        emit textDocumentChanged();
    }

    void highlightBlock(const QString& text) override {
        QTextCharFormat myClassFormat;
        myClassFormat.setFontWeight(QFont::Bold);
        myClassFormat.setForeground(Qt::darkMagenta);

        QRegularExpression expression("0x[0-9]+");
        QRegularExpressionMatchIterator i = expression.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(), match.capturedLength(), myClassFormat);
        }
    }
};

class LayoutHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument* textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)

public:
    LayoutHighlighter(QObject* parent = nullptr) : QSyntaxHighlighter(parent), m_TextDocument(nullptr) {

        QTextCharFormat keyword_format;
        keyword_format.setFontWeight(QFont::Bold);
        keyword_format.setForeground(Qt::darkMagenta);

        QTextCharFormat type_format;
        type_format.setFontWeight(QFont::Bold);
        type_format.setForeground(Qt::darkBlue);

        QTextCharFormat comment_format;
        comment_format.setFontItalic(true);
        comment_format.setForeground(Qt::gray);

        m_rules.push_back({QRegularExpression("\\b(struct|layout)\\b"), keyword_format});
        m_rules.push_back({QRegularExpression("^\\b(b|u|i|f)[0-9]+\\b"), type_format});
        m_rules.push_back({QRegularExpression("//.*"), comment_format});
    }

signals:
    void textDocumentChanged();

protected:
    QQuickTextDocument* m_TextDocument;

    QQuickTextDocument* textDocument() const { return m_TextDocument; }
    void setTextDocument(QQuickTextDocument* textDocument) {
        if (textDocument == m_TextDocument) {
            return;
        }

        m_TextDocument = textDocument;

        QTextDocument* doc = m_TextDocument->textDocument();
        setDocument(doc);

        emit textDocumentChanged();
    }

    void highlightBlock(const QString& text) override {
        for (const auto& rule : m_rules) {
            auto it = rule.pattern.globalMatch(text);
            while (it.hasNext()) {
                QRegularExpressionMatch match = it.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
    }

private:
    std::vector<HighlightingRule> m_rules;
};

#include "main.moc"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    qmlRegisterType<HexHighlighter>("extrico", 1, 0, "HexHighlighter");
    qmlRegisterType<LayoutHighlighter>("extrico", 1, 0, "LayoutHighlighter");

    QQmlApplicationEngine engine;
    const auto fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    engine.rootContext()->setContextProperty("fixedFont", fixedFont);
    engine.load(QUrl((u"qrc:/extrico/app.qml"_qs)));

    return app.exec();
}
