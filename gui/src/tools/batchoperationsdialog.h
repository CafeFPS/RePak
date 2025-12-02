#ifndef BATCHOPERATIONSDIALOG_H
#define BATCHOPERATIONSDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QRadioButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QPushButton>
#include <QPlainTextEdit>

/**
 * Batch Operations dialog for processing multiple PAKs
 */
class BatchOperationsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BatchOperationsDialog(QWidget* parent = nullptr);
    ~BatchOperationsDialog() override;

private slots:
    void addFiles();
    void removeSelected();
    void clearFiles();
    void startOperation();
    void cancelOperation();

private:
    void setupUi();
    void updateButtonState();

private:
    // Operation type
    QRadioButton* m_buildRadio;
    QRadioButton* m_compressRadio;
    QRadioButton* m_decompressRadio;
    QRadioButton* m_validateRadio;

    // File list
    QListWidget* m_fileList;
    QPushButton* m_addButton;
    QPushButton* m_removeButton;
    QPushButton* m_clearButton;

    // Options
    QSpinBox* m_parallelSpin;
    QCheckBox* m_stopOnErrorCheck;
    QCheckBox* m_generateReportCheck;

    // Progress
    QProgressBar* m_progressBar;
    QPlainTextEdit* m_logEdit;

    // Controls
    QPushButton* m_startButton;
    QPushButton* m_cancelButton;

    bool m_isRunning;
};

#endif // BATCHOPERATIONSDIALOG_H
