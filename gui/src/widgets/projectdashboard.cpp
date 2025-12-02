#include "projectdashboard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFrame>

#include "core/project.h"

ProjectDashboard::ProjectDashboard(QWidget* parent)
    : QWidget(parent)
    , m_project(nullptr)
{
    setupUi();
}

ProjectDashboard::~ProjectDashboard()
{
}

void ProjectDashboard::setupUi()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);

    // Title section
    m_titleLabel = new QLabel(tr("No Project Loaded"), this);
    m_titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    layout->addWidget(m_titleLabel);

    // Project info
    QGroupBox* infoGroup = new QGroupBox(tr("Project Information"), this);
    QGridLayout* infoLayout = new QGridLayout(infoGroup);

    infoLayout->addWidget(new QLabel(tr("PAK Version:"), this), 0, 0);
    m_versionLabel = new QLabel("-", this);
    infoLayout->addWidget(m_versionLabel, 0, 1);

    infoLayout->addWidget(new QLabel(tr("Output Directory:"), this), 1, 0);
    m_outputLabel = new QLabel("-", this);
    infoLayout->addWidget(m_outputLabel, 1, 1);

    infoLayout->setColumnStretch(1, 1);
    layout->addWidget(infoGroup);

    // Statistics
    QGroupBox* statsGroup = new QGroupBox(tr("Asset Statistics"), this);
    QGridLayout* statsLayout = new QGridLayout(statsGroup);

    int row = 0;

    statsLayout->addWidget(new QLabel(tr("Total Assets:"), this), row, 0);
    m_totalAssetsLabel = new QLabel("0", this);
    m_totalAssetsLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
    statsLayout->addWidget(m_totalAssetsLabel, row, 1);
    row++;

    // Separator
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    statsLayout->addWidget(line, row, 0, 1, 2);
    row++;

    statsLayout->addWidget(new QLabel(tr("Textures:"), this), row, 0);
    m_textureCountLabel = new QLabel("0", this);
    statsLayout->addWidget(m_textureCountLabel, row, 1);
    row++;

    statsLayout->addWidget(new QLabel(tr("Materials:"), this), row, 0);
    m_materialCountLabel = new QLabel("0", this);
    statsLayout->addWidget(m_materialCountLabel, row, 1);
    row++;

    statsLayout->addWidget(new QLabel(tr("Models:"), this), row, 0);
    m_modelCountLabel = new QLabel("0", this);
    statsLayout->addWidget(m_modelCountLabel, row, 1);
    row++;

    statsLayout->addWidget(new QLabel(tr("Shaders:"), this), row, 0);
    m_shaderCountLabel = new QLabel("0", this);
    statsLayout->addWidget(m_shaderCountLabel, row, 1);
    row++;

    statsLayout->addWidget(new QLabel(tr("Other:"), this), row, 0);
    m_otherCountLabel = new QLabel("0", this);
    statsLayout->addWidget(m_otherCountLabel, row, 1);
    row++;

    // Separator
    QFrame* line2 = new QFrame(this);
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    statsLayout->addWidget(line2, row, 0, 1, 2);
    row++;

    statsLayout->addWidget(new QLabel(tr("Total Size:"), this), row, 0);
    m_totalSizeLabel = new QLabel("-", this);
    statsLayout->addWidget(m_totalSizeLabel, row, 1);

    statsLayout->setColumnStretch(1, 1);
    layout->addWidget(statsGroup);

    layout->addStretch();
}

void ProjectDashboard::setProject(Project* project)
{
    m_project = project;
    refresh();
}

void ProjectDashboard::refresh()
{
    if (!m_project) {
        m_titleLabel->setText(tr("No Project Loaded"));
        m_versionLabel->setText("-");
        m_outputLabel->setText("-");
        m_totalAssetsLabel->setText("0");
        m_textureCountLabel->setText("0");
        m_materialCountLabel->setText("0");
        m_modelCountLabel->setText("0");
        m_shaderCountLabel->setText("0");
        m_otherCountLabel->setText("0");
        m_totalSizeLabel->setText("-");
        return;
    }

    QString name = m_project->name();
    if (name.isEmpty()) {
        name = m_project->fileName();
        if (name.isEmpty()) {
            name = tr("Untitled Project");
        }
    }
    m_titleLabel->setText(name);

    QString version = m_project->version() == PakVersion::Version8 ?
                      "8 (Apex Legends)" : "7 (Titanfall 2)";
    m_versionLabel->setText(version);

    m_outputLabel->setText(m_project->outputDir());

    updateStatistics();
}

void ProjectDashboard::updateStatistics()
{
    if (!m_project) {
        return;
    }

    int total = m_project->assetCount();
    m_totalAssetsLabel->setText(QString::number(total));

    QMap<AssetType, int> counts = m_project->assetCountsByType();

    m_textureCountLabel->setText(QString::number(counts.value(AssetType::Texture, 0)));
    m_materialCountLabel->setText(QString::number(counts.value(AssetType::Material, 0)));
    m_modelCountLabel->setText(QString::number(counts.value(AssetType::Model, 0)));

    int shaders = counts.value(AssetType::Shader, 0) + counts.value(AssetType::ShaderSet, 0);
    m_shaderCountLabel->setText(QString::number(shaders));

    int other = total -
                counts.value(AssetType::Texture, 0) -
                counts.value(AssetType::Material, 0) -
                counts.value(AssetType::Model, 0) -
                shaders;
    m_otherCountLabel->setText(QString::number(other));

    qint64 totalSize = m_project->totalAssetSize();
    QString sizeStr;
    if (totalSize < 0) {
        sizeStr = tr("Unknown");
    } else if (totalSize < 1024) {
        sizeStr = QString("%1 B").arg(totalSize);
    } else if (totalSize < 1024 * 1024) {
        sizeStr = QString("%1 KB").arg(totalSize / 1024.0, 0, 'f', 1);
    } else if (totalSize < 1024 * 1024 * 1024) {
        sizeStr = QString("%1 MB").arg(totalSize / (1024.0 * 1024.0), 0, 'f', 1);
    } else {
        sizeStr = QString("%1 GB").arg(totalSize / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
    }
    m_totalSizeLabel->setText(sizeStr);
}
