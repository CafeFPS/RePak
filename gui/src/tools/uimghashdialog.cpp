#include "uimghashdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QClipboard>
#include <QApplication>

UimgHashDialog::UimgHashDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
}

UimgHashDialog::~UimgHashDialog()
{
}

void UimgHashDialog::setupUi()
{
    setWindowTitle(tr("UI Image Hash Calculator"));
    setMinimumSize(500, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    // Input section
    QGroupBox* inputGroup = new QGroupBox(tr("Calculate Hash"), this);
    QVBoxLayout* inputLayout = new QVBoxLayout(inputGroup);

    QLabel* inputLabel = new QLabel(tr("Image Name:"), this);
    m_inputEdit = new QLineEdit(this);
    m_inputEdit->setPlaceholderText(tr("e.g., rui/hud/healthbar_segment"));
    connect(m_inputEdit, &QLineEdit::returnPressed, this, &UimgHashDialog::calculate);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_calculateButton = new QPushButton(tr("Calculate"), this);
    connect(m_calculateButton, &QPushButton::clicked, this, &UimgHashDialog::calculate);
    buttonLayout->addWidget(m_calculateButton);
    buttonLayout->addStretch();

    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(m_inputEdit);
    inputLayout->addLayout(buttonLayout);

    // Result section
    QHBoxLayout* resultLayout = new QHBoxLayout();
    QLabel* resultLabel = new QLabel(tr("Calculated Hash:"), this);
    m_resultEdit = new QLineEdit(this);
    m_resultEdit->setReadOnly(true);
    m_resultEdit->setFont(QFont("Consolas", 10));

    m_copyButton = new QPushButton(tr("Copy"), this);
    m_copyButton->setEnabled(false);
    connect(m_copyButton, &QPushButton::clicked, this, &UimgHashDialog::copyResult);

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
    connect(clearButton, &QPushButton::clicked, this, &UimgHashDialog::clearHistory);

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

void UimgHashDialog::calculate()
{
    QString name = m_inputEdit->text().trimmed();
    if (name.isEmpty()) {
        return;
    }

    uint32_t hash = calculateHash(name);
    QString hashStr = QString("0x%1").arg(hash, 8, 16, QChar('0')).toUpper();

    m_resultEdit->setText(hashStr);
    m_copyButton->setEnabled(true);

    // Add to history
    QString historyEntry = QString("%1 -> %2").arg(name).arg(hashStr);
    QListWidgetItem* item = new QListWidgetItem(historyEntry);
    item->setData(Qt::UserRole, hashStr);
    m_historyList->insertItem(0, item);

    // Limit history
    while (m_historyList->count() > 50) {
        delete m_historyList->takeItem(m_historyList->count() - 1);
    }
}

void UimgHashDialog::copyResult()
{
    QApplication::clipboard()->setText(m_resultEdit->text());
}

void UimgHashDialog::clearHistory()
{
    m_historyList->clear();
}

uint32_t UimgHashDialog::calculateHash(const QString& name) const
{
    // UI Image hash calculation
    QByteArray data = name.toLower().toUtf8();

    uint32_t hash = 0;
    for (int i = 0; i < data.size(); ++i) {
        hash = (hash * 31) + static_cast<unsigned char>(data[i]);
    }

    return hash;
}
