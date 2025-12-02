#ifndef BUILDLOGWIDGET_H
#define BUILDLOGWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>

/**
 * Build log output widget
 */
class BuildLogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BuildLogWidget(QWidget* parent = nullptr);
    ~BuildLogWidget() override;

public slots:
    void addInfo(const QString& text);
    void addWarning(const QString& text);
    void addError(const QString& text);
    void clear();
    void exportLog();

private:
    void setupUi();
    void appendLine(const QString& text, const QString& color);

private:
    QPlainTextEdit* m_textEdit;
    QPushButton* m_clearButton;
    QPushButton* m_exportButton;
    int m_errorCount;
    int m_warningCount;
};

#endif // BUILDLOGWIDGET_H
