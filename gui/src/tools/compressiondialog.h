#ifndef COMPRESSIONDIALOG_H
#define COMPRESSIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSlider>
#include <QSpinBox>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>

/**
 * Compression dialog for compressing/decompressing PAK files
 */
class CompressionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CompressionDialog(QWidget* parent = nullptr);
    ~CompressionDialog() override;

private slots:
    void browseInput();
    void browseOutput();
    void compress();
    void decompress();

private:
    void setupUi();
    void updateOutputPath();

private:
    QLineEdit* m_inputEdit;
    QLineEdit* m_outputEdit;
    QPushButton* m_inputBrowseButton;
    QPushButton* m_outputBrowseButton;

    QSlider* m_levelSlider;
    QLabel* m_levelLabel;
    QSpinBox* m_workersSpin;

    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;

    QPushButton* m_compressButton;
    QPushButton* m_decompressButton;
};

#endif // COMPRESSIONDIALOG_H
