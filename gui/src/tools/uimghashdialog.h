#ifndef UIMGHASHDIALOG_H
#define UIMGHASHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

/**
 * UI Image Hash Calculator dialog
 */
class UimgHashDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UimgHashDialog(QWidget* parent = nullptr);
    ~UimgHashDialog() override;

private slots:
    void calculate();
    void copyResult();
    void clearHistory();

private:
    void setupUi();
    uint32_t calculateHash(const QString& name) const;

private:
    QLineEdit* m_inputEdit;
    QLineEdit* m_resultEdit;
    QPushButton* m_calculateButton;
    QPushButton* m_copyButton;
    QListWidget* m_historyList;
};

#endif // UIMGHASHDIALOG_H
