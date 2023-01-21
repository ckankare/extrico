#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickTextDocument>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <qnamespace.h>
#include <qobject.h>
#include <qtextcursor.h>
#include <qtextformat.h>

#include "extrico/layout.hpp"
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

    QQuickTextDocument* m_TextDocument;
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

enum class SpanType { Full, Partial, Error };

class Parser : public QObject {
    Q_OBJECT
public:
    explicit Parser(QObject* parent = nullptr) : QObject(parent) {}
signals:
    void layoutChange(const QString& text);
    void dataChange(const QString& text);
    void valueChange(const QString& text);
    void dataUsedChanged(int start, int end, SpanType span_type);
public slots:
    void layoutChanged(const QString& text) {
        const auto raw_text = text.toUtf8().toStdString();

        eto::Parser parser(raw_text);
        m_layout = parser.parse();

        parseData();
    }
    void dataChanged(const QString& text) {
        const auto raw_text = text.toUtf8().toStdString();

        std::tie(m_data, m_mapping) = eto::parse_hex_string(raw_text);

        parseData();
    }

private:
    void parseData() {
        if (!m_layout) {
            m_result = "{}";
            emit dataUsedChanged(0, 0, SpanType::Full);
        } else {
            auto [values, end, parse_result] = m_layout->parse_bits(m_data, eto::Endianess::Big);
            m_result = m_layout->to_string(values);
            const auto end_byte = static_cast<int>(end > 0 ? m_mapping[(end - 1) / 4] + 1 : 0);

            const SpanType span_type = [](auto parse_result) {
                switch (parse_result) {
                    case eto::Layout::ParseResult::Full: return SpanType::Full;
                    case eto::Layout::ParseResult::Excess: return SpanType::Full;
                    case eto::Layout::ParseResult::Error: return SpanType::Error;
                }
            }(parse_result);

            emit dataUsedChanged(0, end_byte, span_type);
        }

        emit valueChange(QString::fromStdString(m_result));
    }

    std::unique_ptr<eto::Layout> m_layout;
    std::vector<uint8_t> m_data;
    std::vector<size_t> m_mapping;
    std::string m_result;
};

class SpanHighlighter : public QObject {
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument* textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)
public:
    explicit SpanHighlighter(QObject* parent = nullptr) : QObject(parent) {}

signals:
    void textDocumentChanged();

public slots:
    void spanChanged(int begin, int end, SpanType span_type) {
        if (m_text_document == nullptr) {
            return;
        }
        QSignalBlocker blocker(this);

        {
            QTextCharFormat clear_format;
            clear_format.setBackground(Qt::white);
            setFormat(0, m_text_document->textDocument()->characterCount() - 1, clear_format);
        }

        if (begin == -1 || end == 0 || begin >= end) {
            return;
        }

        QTextCharFormat highlight_format;
        switch (span_type) {
            case SpanType::Error:
                highlight_format.setForeground(Qt::red);
                highlight_format.setFontWeight(QFont::Bold);
                break;
            case SpanType::Partial:
                highlight_format.setForeground(Qt::yellow);
                highlight_format.setFontWeight(QFont::Bold);
                break;
            case SpanType::Full: highlight_format.setFontWeight(QFont::Bold); break;
        }

        setFormat(begin, end, highlight_format);
    }

protected:
    QQuickTextDocument* textDocument() const { return m_text_document; }
    void setTextDocument(QQuickTextDocument* text_document) {
        if (text_document == m_text_document) {
            return;
        }

        m_text_document = text_document;

        emit textDocumentChanged();
    }

private:
    void setFormat(int begin, int end, const QTextCharFormat& format) {
        auto cursor = QTextCursor(m_text_document->textDocument());
        cursor.setPosition(begin, QTextCursor::MoveAnchor);
        cursor.setPosition(end, QTextCursor::KeepAnchor);
        cursor.setCharFormat(format);
    }
    QQuickTextDocument* m_text_document = nullptr;
};

#include "main.moc"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    app.setOrganizationName("extrico");
    app.setApplicationName("extrico");

    qmlRegisterType<HexHighlighter>("extrico", 1, 0, "HexHighlighter");
    qmlRegisterType<LayoutHighlighter>("extrico", 1, 0, "LayoutHighlighter");
    qmlRegisterType<SpanHighlighter>("extrico", 1, 0, "SpanHighlighter");
    qmlRegisterType<Parser>("extrico", 1, 0, "Parser");

    QQmlApplicationEngine engine;
    const auto fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    engine.rootContext()->setContextProperty("fixedFont", fixedFont);
    engine.load(QUrl((u"qrc:/extrico/app.qml"_qs)));

    return app.exec();
}
