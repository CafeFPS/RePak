#ifndef GUIDCALCULATORDIALOG_H
#define GUIDCALCULATORDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

/**
 * GUID Calculator dialog
 */
class GuidCalculatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GuidCalculatorDialog(QWidget* parent = nullptr);
    ~GuidCalculatorDialog() override;

private slots:
    void calculate();
    void copyResult();
    void clearHistory();

private:
    void setupUi();
    uint64_t calculateGuid(const QString& path) const;

private:
    QLineEdit* m_inputEdit;
    QLineEdit* m_resultEdit;
    QPushButton* m_calculateButton;
    QPushButton* m_copyButton;
    QListWidget* m_historyList;
};

#endif // GUIDCALCULATORDIALOG_H
