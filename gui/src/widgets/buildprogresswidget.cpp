#include "buildprogresswidget.h"

#include <QHBoxLayout>

BuildProgressWidget::BuildProgressWidget(QWidget* parent)
    : QWidget(parent)
    , m_lastCurrent(0)
{
    setupUi();
}

BuildProgressWidget::~BuildProgressWidget()
{
}

void BuildProgressWidget::setupUi()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    m_operationLabel = new QLabel(tr("Ready"), this);
    m_operationLabel->setMinimumWidth(200);
    layout->addWidget(m_operationLabel);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    layout->addWidget(m_progressBar, 1);

    m_timeLabel = new QLabel(this);
    m_timeLabel->setMinimumWidth(150);
    m_timeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(m_timeLabel);

    m_cancelButton = new QPushButton(tr("Cancel"), this);
    m_cancelButton->setMaximumWidth(80);
    connect(m_cancelButton, &QPushButton::clicked, this, &BuildProgressWidget::cancelRequested);
    layout->addWidget(m_cancelButton);

    setStyleSheet(
        "BuildProgressWidget {"
        "  background-color: #2d2d2d;"
        "  border-bottom: 1px solid #3c3c3c;"
        "}"
    );
}

void BuildProgressWidget::setProgress(int current, int total)
{
    if (total > 0) {
        int percent = (current * 100) / total;
        m_progressBar->setValue(percent);
        m_progressBar->setFormat(QString("%1 / %2 (%p%)").arg(current).arg(total));
    } else {
        m_progressBar->setValue(0);
        m_progressBar->setFormat("");
    }

    m_lastCurrent = current;
    updateTimeEstimate();
}

void BuildProgressWidget::setOperation(const QString& operation)
{
    m_operationLabel->setText(operation);
}

void BuildProgressWidget::reset()
{
    m_progressBar->setValue(0);
    m_progressBar->setFormat("");
    m_operationLabel->setText(tr("Starting..."));
    m_timeLabel->clear();
    m_lastCurrent = 0;
    m_timer.start();
}

void BuildProgressWidget::updateTimeEstimate()
{
    if (!m_timer.isValid() || m_lastCurrent <= 0) {
        return;
    }

    qint64 elapsed = m_timer.elapsed();
    qint64 msPerItem = elapsed / m_lastCurrent;
    int total = m_progressBar->maximum();

    if (total > 0 && m_lastCurrent < total) {
        qint64 remaining = msPerItem * (total - m_lastCurrent);
        int seconds = remaining / 1000;
        int minutes = seconds / 60;
        seconds = seconds % 60;

        QString elapsedStr = QString("%1:%2")
                             .arg(elapsed / 60000, 2, 10, QChar('0'))
                             .arg((elapsed / 1000) % 60, 2, 10, QChar('0'));

        QString remainingStr = QString("%1:%2")
                               .arg(minutes, 2, 10, QChar('0'))
                               .arg(seconds, 2, 10, QChar('0'));

        m_timeLabel->setText(QString("Elapsed: %1 | Remaining: ~%2")
                            .arg(elapsedStr)
                            .arg(remainingStr));
    }
}
