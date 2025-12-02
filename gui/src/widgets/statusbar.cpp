#include "statusbar.h"

StatusBar::StatusBar(QWidget* parent)
    : QStatusBar(parent)
{
    m_assetCountLabel = new QLabel(this);
    m_selectionLabel = new QLabel(this);
    m_buildStatusLabel = new QLabel(this);

    addPermanentWidget(m_assetCountLabel);
    addPermanentWidget(m_selectionLabel);
    addPermanentWidget(m_buildStatusLabel);

    setAssetCount(0);
    setSelectionCount(0);
    setBuildStatus("Ready");
}

StatusBar::~StatusBar()
{
}

void StatusBar::setAssetCount(int count)
{
    m_assetCountLabel->setText(tr("Assets: %1").arg(count));
}

void StatusBar::setSelectionCount(int count)
{
    if (count > 0) {
        m_selectionLabel->setText(tr("Selected: %1").arg(count));
        m_selectionLabel->show();
    } else {
        m_selectionLabel->hide();
    }
}

void StatusBar::setBuildStatus(const QString& status)
{
    m_buildStatusLabel->setText(status);
}
