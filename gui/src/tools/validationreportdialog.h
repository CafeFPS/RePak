#ifndef VALIDATIONREPORTDIALOG_H
#define VALIDATIONREPORTDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>

#include "utils/validation.h"

class Project;

/**
 * Validation Report dialog showing project issues
 */
class ValidationReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ValidationReportDialog(Project* project, QWidget* parent = nullptr);
    ~ValidationReportDialog() override;

private slots:
    void revalidate();
    void exportReport();
    void fixSelected();
    void onSelectionChanged();

private:
    void setupUi();
    void runValidation();
    void populateIssueTable();

private:
    Project* m_project;

    QLabel* m_statusLabel;
    QLabel* m_errorCountLabel;
    QLabel* m_warningCountLabel;
    QLabel* m_infoCountLabel;

    QTableWidget* m_issueTable;

    QPushButton* m_revalidateButton;
    QPushButton* m_exportButton;
    QPushButton* m_fixButton;

    QList<Validation::Issue> m_issues;
};

#endif // VALIDATIONREPORTDIALOG_H
