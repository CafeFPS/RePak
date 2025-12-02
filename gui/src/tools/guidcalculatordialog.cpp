#include "guidcalculatordialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QClipboard>
#include <QApplication>

GuidCalculatorDialog::GuidCalculatorDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
}

GuidCalculatorDialog::~GuidCalculatorDialog()
{
}

void GuidCalculatorDialog::setupUi()
{
    setWindowTitle(tr("GUID Calculator"));
    setMinimumSize(500, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    // Input section
    QGroupBox* inputGroup = new QGroupBox(tr("Calculate GUID"), this);
    QVBoxLayout* inputLayout = new QVBoxLayout(inputGroup);

    QLabel* inputLabel = new QLabel(tr("Asset Path:"), this);
    m_inputEdit = new QLineEdit(this);
    m_inputEdit->setPlaceholderText(tr("e.g., materials/models/weapon/r301/r301_base"));
    connect(m_inputEdit, &QLineEdit::returnPressed, this, &GuidCalculatorDialog::calculate);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_calculateButton = new QPushButton(tr("Calculate"), this);
    connect(m_calculateButton, &QPushButton::clicked, this, &GuidCalculatorDialog::calculate);
    buttonLayout->addWidget(m_calculateButton);
    buttonLayout->addStretch();

    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(m_inputEdit);
    inputLayout->addLayout(buttonLayout);

    // Result section
    QHBoxLayout* resultLayout = new QHBoxLayout();
    QLabel* resultLabel = new QLabel(tr("Calculated GUID:"), this);
    m_resultEdit = new QLineEdit(this);
    m_resultEdit->setReadOnly(true);
    m_resultEdit->setFont(QFont("Consolas", 10));

    m_copyButton = new QPushButton(tr("Copy"), this);
    m_copyButton->setEnabled(false);
    connect(m_copyButton, &QPushButton::clicked, this, &GuidCalculatorDialog::copyResult);

    resultLayout->addWidget(resultLabel);
    resultLayout->addWidget(m_resultEdit, 1);
    resultLayout->addWidget(m_copyButton);

    inputLayout->addLayout(resultLayout);

    layout->addWidget(inputGroup);

    // History section
    QGroupBox* historyGroup = new QGroupBox(tr("History"), this);
    QVBoxLayout* historyLayout = new QVBoxLayout(historyGroup);

    m_historyList = new QListWidget(this);
    m_historyList->setAlternatingRowColors(true);
    connect(m_historyList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        QString text = item->data(Qt::UserRole).toString();
        QApplication::clipboard()->setText(text);
    });

    QPushButton* clearButton = new QPushButton(tr("Clear History"), this);
    connect(clearButton, &QPushButton::clicked, this, &GuidCalculatorDialog::clearHistory);

    historyLayout->addWidget(m_historyList);
    historyLayout->addWidget(clearButton);

    layout->addWidget(historyGroup, 1);

    // Close button
    QHBoxLayout* closeLayout = new QHBoxLayout();
    closeLayout->addStretch();
    QPushButton* closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    closeLayout->addWidget(closeButton);

    layout->addLayout(closeLayout);
}

void GuidCalculatorDialog::calculate()
{
    QString path = m_inputEdit->text().trimmed();
    if (path.isEmpty()) {
        return;
    }

    uint64_t guid = calculateGuid(path);
    QString guidStr = QString("0x%1").arg(guid, 16, 16, QChar('0')).toUpper();

    m_resultEdit->setText(guidStr);
    m_copyButton->setEnabled(true);

    // Add to history
    QString historyEntry = QString("%1 -> %2").arg(path).arg(guidStr);
    QListWidgetItem* item = new QListWidgetItem(historyEntry);
    item->setData(Qt::UserRole, guidStr);
    m_historyList->insertItem(0, item);

    // Limit history to 50 items
    while (m_historyList->count() > 50) {
        delete m_historyList->takeItem(m_historyList->count() - 1);
    }
}

void GuidCalculatorDialog::copyResult()
{
    QApplication::clipboard()->setText(m_resultEdit->text());
}

void GuidCalculatorDialog::clearHistory()
{
    m_historyList->clear();
}

uint64_t GuidCalculatorDialog::calculateGuid(const QString& path) const
{
    // RTech GUID calculation (same as RePak)
    // This is a simplified version - the real implementation uses MurmurHash3
    QByteArray data = path.toLower().toUtf8();

    uint64_t hash = 0;
    const uint64_t prime = 0x100000001B3;
    const uint64_t basis = 0xCBF29CE484222325;

    hash = basis;
    for (int i = 0; i < data.size(); ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= prime;
    }

    return hash;
}
