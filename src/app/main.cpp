#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickTextDocument>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

#include "extrico/parse.hpp"
#include "extrico/parser.hpp"

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

class Parser : public QObject {
    Q_OBJECT
public:
    explicit Parser(QObject* parent = 0) : QObject(parent) {}
signals:
    void layoutChange(const QString& text);
    void dataChange(const QString& text);
    void valueChange(const QString& text);
public slots:
    void layoutChanged(const QString& text) {
        const auto raw_text = text.toUtf8().toStdString();

        eto::Parser parser(raw_text);
        m_layout = parser.parse();

        parseData();
    }
    void dataChanged(const QString& text) {
        const auto raw_text = text.toUtf8().toStdString();

        m_data = eto::parse_hex_string(raw_text);

        parseData();
    }

private:
    void parseData() {
        if (!m_layout) {
            m_result = "{}";
        } else {
            auto [values, end] = m_layout->parse_bits(m_data, eto::Endianess::Big);
            m_result = m_layout->to_string(values);
        }

        emit valueChange(QString::fromStdString(m_result));
    }
    std::unique_ptr<eto::Layout> m_layout;
    std::vector<uint8_t> m_data;
    std::string m_result;
};

#include "main.moc"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    qmlRegisterType<HexHighlighter>("extrico", 1, 0, "HexHighlighter");
    qmlRegisterType<LayoutHighlighter>("extrico", 1, 0, "LayoutHighlighter");
    qmlRegisterType<Parser>("extrico", 1, 0, "Parser");

    QQmlApplicationEngine engine;
    const auto fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    engine.rootContext()->setContextProperty("fixedFont", fixedFont);
    engine.load(QUrl((u"qrc:/extrico/app.qml"_qs)));

    return app.exec();
}
