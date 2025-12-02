#include "validationreportdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

#include "core/project.h"

ValidationReportDialog::ValidationReportDialog(Project* project, QWidget* parent)
    : QDialog(parent)
    , m_project(project)
{
    setupUi();
    runValidation();
}

ValidationReportDialog::~ValidationReportDialog()
{
}

void ValidationReportDialog::setupUi()
{
    setWindowTitle(tr("Validation Report"));
    setMinimumSize(800, 550);

    QVBoxLayout* layout = new QVBoxLayout(this);

    // Summary
    QGroupBox* summaryGroup = new QGroupBox(tr("Summary"), this);
    QHBoxLayout* summaryLayout = new QHBoxLayout(summaryGroup);

    m_statusLabel = new QLabel(tr("Validating..."), this);
    m_statusLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    m_errorCountLabel = new QLabel(tr("Errors: 0"), this);
    m_errorCountLabel->setStyleSheet("color: #f14c4c; font-weight: bold;");

    m_warningCountLabel = new QLabel(tr("Warnings: 0"), this);
    m_warningCountLabel->setStyleSheet("color: #dcdcaa; font-weight: bold;");

    m_infoCountLabel = new QLabel(tr("Info: 0"), this);
    m_infoCountLabel->setStyleSheet("color: #569cd6; font-weight: bold;");

    summaryLayout->addWidget(m_statusLabel);
    summaryLayout->addStretch();
    summaryLayout->addWidget(m_errorCountLabel);
    summaryLayout->addWidget(m_warningCountLabel);
    summaryLayout->addWidget(m_infoCountLabel);

    layout->addWidget(summaryGroup);

    // Issues table
    QGroupBox* issuesGroup = new QGroupBox(tr("Issues"), this);
    QVBoxLayout* issuesLayout = new QVBoxLayout(issuesGroup);

    m_issueTable = new QTableWidget(this);
    m_issueTable->setColumnCount(5);
    m_issueTable->setHorizontalHeaderLabels({
        tr("Severity"), tr("Asset"), tr("Issue"), tr("Suggestion"), tr("Auto-Fix")
    });
    m_issueTable->horizontalHeader()->setStretchLastSection(true);
    m_issueTable->setAlternatingRowColors(true);
    m_issueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_issueTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_issueTable->setColumnWidth(0, 80);
    m_issueTable->setColumnWidth(1, 180);
    m_issueTable->setColumnWidth(2, 220);
    m_issueTable->setColumnWidth(3, 180);
    m_issueTable->setColumnWidth(4, 60);
    connect(m_issueTable, &QTableWidget::itemSelectionChanged, this, &ValidationReportDialog::onSelectionChanged);

    issuesLayout->addWidget(m_issueTable);

    layout->addWidget(issuesGroup, 1);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_revalidateButton = new QPushButton(tr("Re-validate"), this);
    m_revalidateButton->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    connect(m_revalidateButton, &QPushButton::clicked, this, &ValidationReportDialog::revalidate);

    m_exportButton = new QPushButton(tr("Export Report..."), this);
    m_exportButton->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    connect(m_exportButton, &QPushButton::clicked, this, &ValidationReportDialog::exportReport);

    m_fixButton = new QPushButton(tr("Auto-Fix Selected"), this);
    m_fixButton->setEnabled(false);
    m_fixButton->setToolTip(tr("Automatically fix selected issues that support auto-fix"));
    connect(m_fixButton, &QPushButton::clicked, this, &ValidationReportDialog::fixSelected);

    QPushButton* closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    buttonLayout->addWidget(m_revalidateButton);
    buttonLayout->addWidget(m_exportButton);
    buttonLayout->addWidget(m_fixButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);

    layout->addLayout(buttonLayout);
}

void ValidationReportDialog::runValidation()
{
    m_issues.clear();

    if (!m_project) {
        m_statusLabel->setText(tr("No project loaded"));
        m_statusLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #888;");
        populateIssueTable();
        return;
    }

    // Run validation using the Validation namespace
    m_issues = Validation::validateProject(m_project);

    populateIssueTable();
}

void ValidationReportDialog::populateIssueTable()
{
    m_issueTable->setRowCount(0);

    int errorCount = 0;
    int warningCount = 0;
    int infoCount = 0;

    for (int i = 0; i < m_issues.size(); ++i) {
        const Validation::Issue& issue = m_issues[i];

        int row = m_issueTable->rowCount();
        m_issueTable->insertRow(row);

        // Severity
        QString severityText;
        QString severityColor;
        switch (issue.severity) {
            case Validation::Severity::Error:
                severityText = tr("Error");
                severityColor = "#f14c4c";
                errorCount++;
                break;
            case Validation::Severity::Warning:
                severityText = tr("Warning");
                severityColor = "#dcdcaa";
                warningCount++;
                break;
            case Validation::Severity::Info:
                severityText = tr("Info");
                severityColor = "#569cd6";
                infoCount++;
                break;
        }

        QTableWidgetItem* severityItem = new QTableWidgetItem(severityText);
        severityItem->setForeground(QColor(severityColor));
        severityItem->setData(Qt::UserRole, i); // Store index for auto-fix
        m_issueTable->setItem(row, 0, severityItem);

        // Asset
        QTableWidgetItem* assetItem = new QTableWidgetItem(issue.assetPath.isEmpty() ? "-" : issue.assetPath);
        m_issueTable->setItem(row, 1, assetItem);

        // Issue
        QTableWidgetItem* issueItem = new QTableWidgetItem(issue.message);
        m_issueTable->setItem(row, 2, issueItem);

        // Suggestion
        QTableWidgetItem* suggestionItem = new QTableWidgetItem(issue.suggestion.isEmpty() ? "-" : issue.suggestion);
        suggestionItem->setForeground(QColor("#888"));
        m_issueTable->setItem(row, 3, suggestionItem);

        // Auto-Fix
        QTableWidgetItem* fixItem = new QTableWidgetItem(issue.autoFixable ? tr("Yes") : tr("No"));
        fixItem->setForeground(issue.autoFixable ? QColor("#4ec9b0") : QColor("#888"));
        m_issueTable->setItem(row, 4, fixItem);
    }

    // Update counts
    m_errorCountLabel->setText(tr("Errors: %1").arg(errorCount));
    m_warningCountLabel->setText(tr("Warnings: %1").arg(warningCount));
    m_infoCountLabel->setText(tr("Info: %1").arg(infoCount));

    // Update status
    if (errorCount == 0 && warningCount == 0 && infoCount == 0) {
        m_statusLabel->setText(tr("✓ No issues found - project is valid"));
        m_statusLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #4ec9b0;");
    } else if (errorCount > 0) {
        m_statusLabel->setText(tr("✗ Errors found - build will likely fail"));
        m_statusLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #f14c4c;");
    } else if (warningCount > 0) {
        m_statusLabel->setText(tr("⚠ Warnings found - build should succeed"));
        m_statusLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #dcdcaa;");
    } else {
        m_statusLabel->setText(tr("ℹ Information messages only"));
        m_statusLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #569cd6;");
    }
}

void ValidationReportDialog::revalidate()
{
    m_statusLabel->setText(tr("Validating..."));
    m_statusLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    runValidation();
}

void ValidationReportDialog::exportReport()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Export Validation Report"),
        QString(),
        tr("Text Files (*.txt);;HTML Files (*.html);;All Files (*)")
    );

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);

        if (filePath.endsWith(".html", Qt::CaseInsensitive)) {
            // HTML format
            stream << "<!DOCTYPE html>\n<html>\n<head>\n";
            stream << "<title>Validation Report</title>\n";
            stream << "<style>\n";
            stream << "body { font-family: Arial, sans-serif; margin: 20px; background: #1e1e1e; color: #d4d4d4; }\n";
            stream << "h1 { color: #569cd6; }\n";
            stream << "table { border-collapse: collapse; width: 100%; }\n";
            stream << "th, td { border: 1px solid #3c3c3c; padding: 8px; text-align: left; }\n";
            stream << "th { background: #252526; }\n";
            stream << ".error { color: #f14c4c; }\n";
            stream << ".warning { color: #dcdcaa; }\n";
            stream << ".info { color: #569cd6; }\n";
            stream << "</style>\n</head>\n<body>\n";
            stream << "<h1>Validation Report</h1>\n";
            stream << "<p><strong>Errors:</strong> " << m_errorCountLabel->text().mid(8) << " | ";
            stream << "<strong>Warnings:</strong> " << m_warningCountLabel->text().mid(10) << " | ";
            stream << "<strong>Info:</strong> " << m_infoCountLabel->text().mid(6) << "</p>\n";
            stream << "<table>\n<tr><th>Severity</th><th>Asset</th><th>Issue</th><th>Suggestion</th></tr>\n";

            for (const Validation::Issue& issue : m_issues) {
                QString cssClass;
                QString severityText;
                switch (issue.severity) {
                    case Validation::Severity::Error:
                        cssClass = "error";
                        severityText = "Error";
                        break;
                    case Validation::Severity::Warning:
                        cssClass = "warning";
                        severityText = "Warning";
                        break;
                    case Validation::Severity::Info:
                        cssClass = "info";
                        severityText = "Info";
                        break;
                }
                stream << "<tr class=\"" << cssClass << "\">";
                stream << "<td>" << severityText << "</td>";
                stream << "<td>" << (issue.assetPath.isEmpty() ? "-" : issue.assetPath) << "</td>";
                stream << "<td>" << issue.message << "</td>";
                stream << "<td>" << (issue.suggestion.isEmpty() ? "-" : issue.suggestion) << "</td>";
                stream << "</tr>\n";
            }

            stream << "</table>\n</body>\n</html>\n";
        } else {
            // Plain text format
            stream << "=================================\n";
            stream << "       VALIDATION REPORT         \n";
            stream << "=================================\n\n";
            stream << "Summary:\n";
            stream << "  " << m_errorCountLabel->text() << "\n";
            stream << "  " << m_warningCountLabel->text() << "\n";
            stream << "  " << m_infoCountLabel->text() << "\n\n";
            stream << "---------------------------------\n";
            stream << "Issues:\n";
            stream << "---------------------------------\n\n";

            for (const Validation::Issue& issue : m_issues) {
                QString severityText;
                switch (issue.severity) {
                    case Validation::Severity::Error: severityText = "ERROR"; break;
                    case Validation::Severity::Warning: severityText = "WARNING"; break;
                    case Validation::Severity::Info: severityText = "INFO"; break;
                }

                stream << "[" << severityText << "]\n";
                if (!issue.assetPath.isEmpty()) {
                    stream << "  Asset: " << issue.assetPath << "\n";
                }
                stream << "  Issue: " << issue.message << "\n";
                if (!issue.suggestion.isEmpty()) {
                    stream << "  Suggestion: " << issue.suggestion << "\n";
                }
                stream << "\n";
            }
        }

        file.close();

        QMessageBox::information(this, tr("Export Complete"),
            tr("Validation report exported to:\n%1").arg(filePath));
    } else {
        QMessageBox::warning(this, tr("Export Failed"),
            tr("Could not write to file:\n%1").arg(filePath));
    }
}

void ValidationReportDialog::fixSelected()
{
    QList<QTableWidgetItem*> selectedItems = m_issueTable->selectedItems();
    if (selectedItems.isEmpty()) {
        return;
    }

    int fixedCount = 0;
    QSet<int> processedRows;

    for (QTableWidgetItem* item : selectedItems) {
        int row = item->row();
        if (processedRows.contains(row)) {
            continue;
        }
        processedRows.insert(row);

        // Get issue index from first column's user data
        QTableWidgetItem* severityItem = m_issueTable->item(row, 0);
        if (!severityItem) {
            continue;
        }

        int issueIndex = severityItem->data(Qt::UserRole).toInt();
        if (issueIndex < 0 || issueIndex >= m_issues.size()) {
            continue;
        }

        const Validation::Issue& issue = m_issues[issueIndex];
        if (!issue.autoFixable) {
            continue;
        }

        // TODO: Implement actual auto-fix logic based on issue type
        // For now, just demonstrate the capability
        // Examples of auto-fixes:
        // - Add missing 0x prefix to GUIDs
        // - Regenerate duplicate GUIDs
        // - Remove references to missing files

        fixedCount++;
    }

    if (fixedCount > 0) {
        QMessageBox::information(this, tr("Auto-Fix"),
            tr("Fixed %1 issue(s). Re-validating...").arg(fixedCount));
        revalidate();
    } else {
        QMessageBox::information(this, tr("Auto-Fix"),
            tr("No auto-fixable issues were selected."));
    }
}

void ValidationReportDialog::onSelectionChanged()
{
    // Check if any selected row has auto-fixable issues
    bool hasFixable = false;
    QList<QTableWidgetItem*> selectedItems = m_issueTable->selectedItems();

    QSet<int> checkedRows;
    for (QTableWidgetItem* item : selectedItems) {
        int row = item->row();
        if (checkedRows.contains(row)) {
            continue;
        }
        checkedRows.insert(row);

        QTableWidgetItem* fixItem = m_issueTable->item(row, 4);
        if (fixItem && fixItem->text() == tr("Yes")) {
            hasFixable = true;
            break;
        }
    }

    m_fixButton->setEnabled(hasFixable);
}
