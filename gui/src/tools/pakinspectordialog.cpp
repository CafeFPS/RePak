#include "pakinspectordialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QHeaderView>
#include <QMessageBox>

PakInspectorDialog::PakInspectorDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
}

PakInspectorDialog::~PakInspectorDialog()
{
}

void PakInspectorDialog::setupUi()
{
    setWindowTitle(tr("PAK Inspector"));
    setMinimumSize(800, 600);

    QVBoxLayout* layout = new QVBoxLayout(this);

    // File selection
    QHBoxLayout* fileLayout = new QHBoxLayout();
    m_filePathLabel = new QLabel(tr("No file selected"), this);
    m_filePathLabel->setStyleSheet("color: #888;");

    m_openButton = new QPushButton(tr("Open PAK..."), this);
    connect(m_openButton, &QPushButton::clicked, this, &PakInspectorDialog::openPak);

    fileLayout->addWidget(m_filePathLabel, 1);
    fileLayout->addWidget(m_openButton);

    layout->addLayout(fileLayout);

    // Header info
    QGroupBox* headerGroup = new QGroupBox(tr("Header Information"), this);
    QGridLayout* headerLayout = new QGridLayout(headerGroup);

    int row = 0;
    headerLayout->addWidget(new QLabel(tr("Magic:"), this), row, 0);
    m_magicLabel = new QLabel("-", this);
    headerLayout->addWidget(m_magicLabel, row, 1);

    headerLayout->addWidget(new QLabel(tr("Version:"), this), row, 2);
    m_versionLabel = new QLabel("-", this);
    headerLayout->addWidget(m_versionLabel, row, 3);

    row++;
    headerLayout->addWidget(new QLabel(tr("Flags:"), this), row, 0);
    m_flagsLabel = new QLabel("-", this);
    headerLayout->addWidget(m_flagsLabel, row, 1);

    headerLayout->addWidget(new QLabel(tr("Assets:"), this), row, 2);
    m_assetCountLabel = new QLabel("-", this);
    headerLayout->addWidget(m_assetCountLabel, row, 3);

    row++;
    headerLayout->addWidget(new QLabel(tr("Compressed Size:"), this), row, 0);
    m_compressedSizeLabel = new QLabel("-", this);
    headerLayout->addWidget(m_compressedSizeLabel, row, 1);

    headerLayout->addWidget(new QLabel(tr("Decompressed Size:"), this), row, 2);
    m_decompressedSizeLabel = new QLabel("-", this);
    headerLayout->addWidget(m_decompressedSizeLabel, row, 3);

    row++;
    headerLayout->addWidget(new QLabel(tr("Pages:"), this), row, 0);
    m_pageCountLabel = new QLabel("-", this);
    headerLayout->addWidget(m_pageCountLabel, row, 1);

    headerLayout->addWidget(new QLabel(tr("Slabs:"), this), row, 2);
    m_slabCountLabel = new QLabel("-", this);
    headerLayout->addWidget(m_slabCountLabel, row, 3);

    layout->addWidget(headerGroup);

    // Asset table
    QGroupBox* assetsGroup = new QGroupBox(tr("Assets"), this);
    QVBoxLayout* assetsLayout = new QVBoxLayout(assetsGroup);

    m_assetTable = new QTableWidget(this);
    m_assetTable->setColumnCount(4);
    m_assetTable->setHorizontalHeaderLabels({tr("GUID"), tr("Type"), tr("Version"), tr("Size")});
    m_assetTable->horizontalHeader()->setStretchLastSection(true);
    m_assetTable->setAlternatingRowColors(true);
    m_assetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_assetTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    assetsLayout->addWidget(m_assetTable);

    m_exportButton = new QPushButton(tr("Export Asset List..."), this);
    m_exportButton->setEnabled(false);
    connect(m_exportButton, &QPushButton::clicked, this, &PakInspectorDialog::exportAssetList);

    assetsLayout->addWidget(m_exportButton);

    layout->addWidget(assetsGroup, 1);

    // Close button
    QHBoxLayout* closeLayout = new QHBoxLayout();
    closeLayout->addStretch();
    QPushButton* closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    closeLayout->addWidget(closeButton);

    layout->addLayout(closeLayout);
}

void PakInspectorDialog::openPak()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open PAK File"),
        QString(),
        tr("RPak Files (*.rpak);;All Files (*)")
    );

    if (!filePath.isEmpty()) {
        loadPakInfo(filePath);
    }
}

void PakInspectorDialog::loadPakInfo(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to open file: %1").arg(filePath));
        return;
    }

    // Read header
    QByteArray header = file.read(128);
    if (header.size() < 88) {
        QMessageBox::critical(this, tr("Error"), tr("Invalid PAK file - too small"));
        file.close();
        return;
    }

    // Parse magic
    uint32_t magic = *reinterpret_cast<const uint32_t*>(header.constData());
    if (magic != 0x6B615052) { // 'RPak'
        QMessageBox::critical(this, tr("Error"), tr("Invalid PAK file - bad magic"));
        file.close();
        return;
    }

    m_currentFilePath = filePath;
    m_filePathLabel->setText(filePath);
    m_filePathLabel->setStyleSheet("");

    // Parse header fields
    uint16_t version = *reinterpret_cast<const uint16_t*>(header.constData() + 4);
    uint16_t flags = *reinterpret_cast<const uint16_t*>(header.constData() + 6);
    uint64_t compressedSize = *reinterpret_cast<const uint64_t*>(header.constData() + 0x18);
    uint64_t decompressedSize = *reinterpret_cast<const uint64_t*>(header.constData() + 0x30);
    uint16_t slabCount = *reinterpret_cast<const uint16_t*>(header.constData() + 0x4C);
    uint16_t pageCount = *reinterpret_cast<const uint16_t*>(header.constData() + 0x4E);
    uint32_t assetCount = *reinterpret_cast<const uint32_t*>(header.constData() + 0x58);

    m_magicLabel->setText("RPak (0x6B615052)");
    m_versionLabel->setText(QString::number(version));

    QString flagsStr = QString("0x%1").arg(flags, 4, 16, QChar('0'));
    if (flags & 0x8000) flagsStr += " (ZStd)";
    else if (flags & 0x0200) flagsStr += " (Oodle)";
    else if (flags & 0x0100) flagsStr += " (RTech)";
    m_flagsLabel->setText(flagsStr);

    m_assetCountLabel->setText(QString::number(assetCount));

    auto formatSize = [](uint64_t size) -> QString {
        if (size < 1024) return QString("%1 B").arg(size);
        if (size < 1024*1024) return QString("%1 KB").arg(size / 1024.0, 0, 'f', 1);
        if (size < 1024*1024*1024) return QString("%1 MB").arg(size / (1024.0*1024.0), 0, 'f', 1);
        return QString("%1 GB").arg(size / (1024.0*1024.0*1024.0), 0, 'f', 2);
    };

    m_compressedSizeLabel->setText(formatSize(compressedSize));
    m_decompressedSizeLabel->setText(formatSize(decompressedSize));
    m_pageCountLabel->setText(QString::number(pageCount));
    m_slabCountLabel->setText(QString::number(slabCount));

    // Note: Full asset parsing would require decompression for compressed PAKs
    // This is a simplified view showing header info only
    m_assetTable->setRowCount(0);
    m_exportButton->setEnabled(true);

    file.close();
}

void PakInspectorDialog::exportAssetList()
{
    if (m_currentFilePath.isEmpty()) {
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Export Asset List"),
        QString(),
        tr("CSV Files (*.csv);;All Files (*)")
    );

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << "GUID,Type,Version,Size\n";

        for (int row = 0; row < m_assetTable->rowCount(); ++row) {
            stream << m_assetTable->item(row, 0)->text() << ","
                   << m_assetTable->item(row, 1)->text() << ","
                   << m_assetTable->item(row, 2)->text() << ","
                   << m_assetTable->item(row, 3)->text() << "\n";
        }

        file.close();
    }
}

void PakInspectorDialog::clearInfo()
{
    m_filePathLabel->setText(tr("No file selected"));
    m_filePathLabel->setStyleSheet("color: #888;");
    m_magicLabel->setText("-");
    m_versionLabel->setText("-");
    m_flagsLabel->setText("-");
    m_compressedSizeLabel->setText("-");
    m_decompressedSizeLabel->setText("-");
    m_assetCountLabel->setText("-");
    m_pageCountLabel->setText("-");
    m_slabCountLabel->setText("-");
    m_assetTable->setRowCount(0);
    m_exportButton->setEnabled(false);
    m_currentFilePath.clear();
}
