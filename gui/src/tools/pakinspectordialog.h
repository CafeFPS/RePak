#ifndef PAKINSPECTORDIALOG_H
#define PAKINSPECTORDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>

/**
 * PAK Inspector dialog for examining RPak files
 */
class PakInspectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PakInspectorDialog(QWidget* parent = nullptr);
    ~PakInspectorDialog() override;

private slots:
    void openPak();
    void exportAssetList();

private:
    void setupUi();
    void loadPakInfo(const QString& filePath);
    void clearInfo();

private:
    // Header info
    QLabel* m_filePathLabel;
    QLabel* m_magicLabel;
    QLabel* m_versionLabel;
    QLabel* m_flagsLabel;
    QLabel* m_compressedSizeLabel;
    QLabel* m_decompressedSizeLabel;
    QLabel* m_assetCountLabel;
    QLabel* m_pageCountLabel;
    QLabel* m_slabCountLabel;

    // Asset table
    QTableWidget* m_assetTable;

    QPushButton* m_openButton;
    QPushButton* m_exportButton;

    QString m_currentFilePath;
};

#endif // PAKINSPECTORDIALOG_H
