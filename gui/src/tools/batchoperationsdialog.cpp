#include "batchoperationsdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QFileDialog>
#include <QLabel>

BatchOperationsDialog::BatchOperationsDialog(QWidget* parent)
    : QDialog(parent)
    , m_isRunning(false)
{
    setupUi();
}

BatchOperationsDialog::~BatchOperationsDialog()
{
}

void BatchOperationsDialog::setupUi()
{
    setWindowTitle(tr("Batch Operations"));
    setMinimumSize(700, 600);

    QVBoxLayout* layout = new QVBoxLayout(this);

    // Operation type
    QGroupBox* opGroup = new QGroupBox(tr("Operation Type"), this);
    QHBoxLayout* opLayout = new QHBoxLayout(opGroup);

    m_buildRadio = new QRadioButton(tr("Build Multiple PAKs"), this);
    m_buildRadio->setChecked(true);
    m_compressRadio = new QRadioButton(tr("Compress PAKs"), this);
    m_decompressRadio = new QRadioButton(tr("Decompress PAKs"), this);
    m_validateRadio = new QRadioButton(tr("Validate Projects"), this);

    opLayout->addWidget(m_buildRadio);
    opLayout->addWidget(m_compressRadio);
    opLayout->addWidget(m_decompressRadio);
    opLayout->addWidget(m_validateRadio);

    layout->addWidget(opGroup);

    // File list
    QGroupBox* filesGroup = new QGroupBox(tr("Input Files"), this);
    QVBoxLayout* filesLayout = new QVBoxLayout(filesGroup);

    m_fileList = new QListWidget(this);
    m_fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_fileList->setAlternatingRowColors(true);

    QHBoxLayout* fileButtonLayout = new QHBoxLayout();
    m_addButton = new QPushButton(tr("Add Files..."), this);
    connect(m_addButton, &QPushButton::clicked, this, &BatchOperationsDialog::addFiles);
    m_removeButton = new QPushButton(tr("Remove"), this);
    connect(m_removeButton, &QPushButton::clicked, this, &BatchOperationsDialog::removeSelected);
    m_clearButton = new QPushButton(tr("Clear"), this);
    connect(m_clearButton, &QPushButton::clicked, this, &BatchOperationsDialog::clearFiles);

    fileButtonLayout->addWidget(m_addButton);
    fileButtonLayout->addWidget(m_removeButton);
    fileButtonLayout->addWidget(m_clearButton);
    fileButtonLayout->addStretch();

    filesLayout->addWidget(m_fileList);
    filesLayout->addLayout(fileButtonLayout);

    layout->addWidget(filesGroup);

    // Options
    QGroupBox* optionsGroup = new QGroupBox(tr("Options"), this);
    QGridLayout* optionsLayout = new QGridLayout(optionsGroup);

    optionsLayout->addWidget(new QLabel(tr("Parallel Jobs:"), this), 0, 0);
    m_parallelSpin = new QSpinBox(this);
    m_parallelSpin->setRange(1, 16);
    m_parallelSpin->setValue(4);
    optionsLayout->addWidget(m_parallelSpin, 0, 1);

    m_stopOnErrorCheck = new QCheckBox(tr("Stop on First Error"), this);
    m_stopOnErrorCheck->setChecked(true);
    optionsLayout->addWidget(m_stopOnErrorCheck, 0, 2);

    m_generateReportCheck = new QCheckBox(tr("Generate Summary Report"), this);
    m_generateReportCheck->setChecked(true);
    optionsLayout->addWidget(m_generateReportCheck, 0, 3);

    layout->addWidget(optionsGroup);

    // Progress
    QGroupBox* progressGroup = new QGroupBox(tr("Progress"), this);
    QVBoxLayout* progressLayout = new QVBoxLayout(progressGroup);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);

    m_logEdit = new QPlainTextEdit(this);
    m_logEdit->setReadOnly(true);
    m_logEdit->setMaximumHeight(150);
    m_logEdit->setFont(QFont("Consolas", 9));

    progressLayout->addWidget(m_progressBar);
    progressLayout->addWidget(m_logEdit);

    layout->addWidget(progressGroup);

    // Control buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_startButton = new QPushButton(tr("Start"), this);
    connect(m_startButton, &QPushButton::clicked, this, &BatchOperationsDialog::startOperation);

    m_cancelButton = new QPushButton(tr("Cancel"), this);
    m_cancelButton->setEnabled(false);
    connect(m_cancelButton, &QPushButton::clicked, this, &BatchOperationsDialog::cancelOperation);

    QPushButton* closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_startButton);
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(closeButton);

    layout->addLayout(buttonLayout);

    updateButtonState();
}

void BatchOperationsDialog::addFiles()
{
    QString filter;
    if (m_buildRadio->isChecked() || m_validateRadio->isChecked()) {
        filter = tr("Project Files (*.json);;All Files (*)");
    } else {
        filter = tr("PAK Files (*.rpak);;All Files (*)");
    }

    QStringList files = QFileDialog::getOpenFileNames(
        this,
        tr("Add Files"),
        QString(),
        filter
    );

    for (const QString& file : files) {
        // Avoid duplicates
        bool exists = false;
        for (int i = 0; i < m_fileList->count(); ++i) {
            if (m_fileList->item(i)->text() == file) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            QListWidgetItem* item = new QListWidgetItem(file);
            item->setCheckState(Qt::Checked);
            m_fileList->addItem(item);
        }
    }

    updateButtonState();
}

void BatchOperationsDialog::removeSelected()
{
    QList<QListWidgetItem*> selected = m_fileList->selectedItems();
    for (QListWidgetItem* item : selected) {
        delete item;
    }
    updateButtonState();
}

void BatchOperationsDialog::clearFiles()
{
    m_fileList->clear();
    updateButtonState();
}

void BatchOperationsDialog::startOperation()
{
    if (m_fileList->count() == 0) {
        return;
    }

    m_isRunning = true;
    m_startButton->setEnabled(false);
    m_cancelButton->setEnabled(true);
    m_logEdit->clear();
    m_progressBar->setValue(0);

    // TODO: Implement actual batch processing
    m_logEdit->appendPlainText("Batch processing not yet implemented.");

    m_isRunning = false;
    m_startButton->setEnabled(true);
    m_cancelButton->setEnabled(false);
    m_progressBar->setValue(100);
}

void BatchOperationsDialog::cancelOperation()
{
    m_isRunning = false;
    m_startButton->setEnabled(true);
    m_cancelButton->setEnabled(false);
    m_logEdit->appendPlainText("Operation cancelled.");
}

void BatchOperationsDialog::updateButtonState()
{
    bool hasFiles = m_fileList->count() > 0;
    m_startButton->setEnabled(hasFiles && !m_isRunning);
    m_removeButton->setEnabled(hasFiles);
    m_clearButton->setEnabled(hasFiles);
}
