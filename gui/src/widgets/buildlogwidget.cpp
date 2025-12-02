#include "buildlogwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>

BuildLogWidget::BuildLogWidget(QWidget* parent)
    : QWidget(parent)
    , m_errorCount(0)
    , m_warningCount(0)
{
    setupUi();
}

BuildLogWidget::~BuildLogWidget()
{
}

void BuildLogWidget::setupUi()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    // Header
    QWidget* header = new QWidget(this);
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(8, 4, 8, 4);

    QLabel* label = new QLabel(tr("Build Log"), this);
    label->setStyleSheet("font-weight: bold;");
    headerLayout->addWidget(label);

    headerLayout->addStretch();

    m_clearButton = new QPushButton(tr("Clear"), this);
    m_clearButton->setMaximumWidth(80);
    connect(m_clearButton, &QPushButton::clicked, this, &BuildLogWidget::clear);
    headerLayout->addWidget(m_clearButton);

    m_exportButton = new QPushButton(tr("Export"), this);
    m_exportButton->setMaximumWidth(80);
    connect(m_exportButton, &QPushButton::clicked, this, &BuildLogWidget::exportLog);
    headerLayout->addWidget(m_exportButton);

    layout->addWidget(header);

    // Text edit
    m_textEdit = new QPlainTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setFont(QFont("Consolas", 9));
    m_textEdit->setStyleSheet(
        "QPlainTextEdit {"
        "  background-color: #1e1e1e;"
        "  color: #d4d4d4;"
        "  border: 1px solid #3c3c3c;"
        "}"
    );
    layout->addWidget(m_textEdit);
}

void BuildLogWidget::addInfo(const QString& text)
{
    appendLine(text, "#d4d4d4");
}

void BuildLogWidget::addWarning(const QString& text)
{
    m_warningCount++;
    appendLine("[WARNING] " + text, "#dcdcaa");
}

void BuildLogWidget::addError(const QString& text)
{
    m_errorCount++;
    appendLine("[ERROR] " + text, "#f14c4c");
}

void BuildLogWidget::clear()
{
    m_textEdit->clear();
    m_errorCount = 0;
    m_warningCount = 0;
}

void BuildLogWidget::exportLog()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Export Build Log"),
        QString("build_log_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        tr("Text Files (*.txt);;All Files (*)")
    );

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << m_textEdit->toPlainText();
        file.close();
    }
}

void BuildLogWidget::appendLine(const QString& text, const QString& color)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString html = QString("<span style='color: %1'>[%2] %3</span>")
                   .arg(color)
                   .arg(timestamp)
                   .arg(text.toHtmlEscaped());

    m_textEdit->appendHtml(html);
}
