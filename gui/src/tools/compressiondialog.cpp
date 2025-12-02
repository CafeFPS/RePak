#include "compressiondialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QFileDialog>
#include <QFileInfo>

CompressionDialog::CompressionDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
}

CompressionDialog::~CompressionDialog()
{
}

void CompressionDialog::setupUi()
{
    setWindowTitle(tr("PAK Compression Tool"));
    setMinimumSize(500, 350);

    QVBoxLayout* layout = new QVBoxLayout(this);

    // File selection
    QGroupBox* fileGroup = new QGroupBox(tr("Files"), this);
    QGridLayout* fileLayout = new QGridLayout(fileGroup);

    fileLayout->addWidget(new QLabel(tr("Input:"), this), 0, 0);
    m_inputEdit = new QLineEdit(this);
    fileLayout->addWidget(m_inputEdit, 0, 1);
    m_inputBrowseButton = new QPushButton(tr("Browse..."), this);
    connect(m_inputBrowseButton, &QPushButton::clicked, this, &CompressionDialog::browseInput);
    fileLayout->addWidget(m_inputBrowseButton, 0, 2);

    fileLayout->addWidget(new QLabel(tr("Output:"), this), 1, 0);
    m_outputEdit = new QLineEdit(this);
    fileLayout->addWidget(m_outputEdit, 1, 1);
    m_outputBrowseButton = new QPushButton(tr("Browse..."), this);
    connect(m_outputBrowseButton, &QPushButton::clicked, this, &CompressionDialog::browseOutput);
    fileLayout->addWidget(m_outputBrowseButton, 1, 2);

    layout->addWidget(fileGroup);

    // Compression options
    QGroupBox* optionsGroup = new QGroupBox(tr("Compression Options"), this);
    QGridLayout* optionsLayout = new QGridLayout(optionsGroup);

    optionsLayout->addWidget(new QLabel(tr("Level:"), this), 0, 0);
    m_levelSlider = new QSlider(Qt::Horizontal, this);
    m_levelSlider->setRange(-5, 22);
    m_levelSlider->setValue(6);
    m_levelSlider->setTickPosition(QSlider::TicksBelow);
    m_levelSlider->setTickInterval(5);
    optionsLayout->addWidget(m_levelSlider, 0, 1);
    m_levelLabel = new QLabel("6", this);
    m_levelLabel->setMinimumWidth(30);
    optionsLayout->addWidget(m_levelLabel, 0, 2);

    connect(m_levelSlider, &QSlider::valueChanged, this, [this](int value) {
        m_levelLabel->setText(QString::number(value));
    });

    optionsLayout->addWidget(new QLabel(tr("Workers:"), this), 1, 0);
    m_workersSpin = new QSpinBox(this);
    m_workersSpin->setRange(1, 64);
    m_workersSpin->setValue(16);
    optionsLayout->addWidget(m_workersSpin, 1, 1, 1, 2);

    layout->addWidget(optionsGroup);

    // Progress
    QGroupBox* progressGroup = new QGroupBox(tr("Progress"), this);
    QVBoxLayout* progressLayout = new QVBoxLayout(progressGroup);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);

    m_statusLabel = new QLabel(tr("Ready"), this);

    progressLayout->addWidget(m_progressBar);
    progressLayout->addWidget(m_statusLabel);

    layout->addWidget(progressGroup);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_compressButton = new QPushButton(tr("Compress"), this);
    connect(m_compressButton, &QPushButton::clicked, this, &CompressionDialog::compress);

    m_decompressButton = new QPushButton(tr("Decompress"), this);
    connect(m_decompressButton, &QPushButton::clicked, this, &CompressionDialog::decompress);

    QPushButton* closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    buttonLayout->addWidget(m_compressButton);
    buttonLayout->addWidget(m_decompressButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);

    layout->addLayout(buttonLayout);
}

void CompressionDialog::browseInput()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Select PAK File"),
        QString(),
        tr("RPak Files (*.rpak);;All Files (*)")
    );

    if (!filePath.isEmpty()) {
        m_inputEdit->setText(filePath);
        updateOutputPath();
    }
}

void CompressionDialog::browseOutput()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Save As"),
        m_outputEdit->text(),
        tr("RPak Files (*.rpak);;All Files (*)")
    );

    if (!filePath.isEmpty()) {
        m_outputEdit->setText(filePath);
    }
}

void CompressionDialog::updateOutputPath()
{
    QString input = m_inputEdit->text();
    if (input.isEmpty()) {
        return;
    }

    QFileInfo fi(input);
    QString output = fi.absolutePath() + "/" + fi.completeBaseName() + "_compressed." + fi.suffix();
    m_outputEdit->setText(output);
}

void CompressionDialog::compress()
{
    // TODO: Implement compression using BuildManager or separate process
    m_statusLabel->setText(tr("Compression not yet implemented"));
}

void CompressionDialog::decompress()
{
    // TODO: Implement decompression
    m_statusLabel->setText(tr("Decompression not yet implemented"));
}
