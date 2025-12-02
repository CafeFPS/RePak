#include "modeleditor.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QScrollArea>
#include <QHeaderView>

#include "core/project.h"

ModelEditor::ModelEditor(QWidget* parent)
    : AssetEditorBase(parent)
{
    setupUi();
}

ModelEditor::~ModelEditor()
{
}

void ModelEditor::setupUi()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* scrollContent = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(scrollContent);

    // Title
    QLabel* titleLabel = new QLabel(tr("Model Asset"), this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    layout->addWidget(titleLabel);

    // Basic Properties Group
    QGroupBox* basicGroup = new QGroupBox(tr("Basic Properties"), this);
    QGridLayout* basicLayout = new QGridLayout(basicGroup);
    int row = 0;

    basicLayout->addWidget(new QLabel(tr("Path:"), this), row, 0);
    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setReadOnly(true);
    basicLayout->addWidget(m_pathEdit, row, 1, 1, 2);
    row++;

    basicLayout->addWidget(new QLabel(tr("GUID:"), this), row, 0);
    m_guidEdit = new QLineEdit(this);
    m_guidEdit->setPlaceholderText("0x0000000000000000");
    connect(m_guidEdit, &QLineEdit::textChanged, this, [this]() {
        setModified(true);
    });
    basicLayout->addWidget(m_guidEdit, row, 1, 1, 2);
    row++;

    basicLayout->addWidget(new QLabel(tr("Animation Rig:"), this), row, 0);
    m_animRigEdit = new QLineEdit(this);
    m_animRigEdit->setPlaceholderText(tr("Optional - arig asset path"));
    connect(m_animRigEdit, &QLineEdit::textChanged, this, [this]() {
        setModified(true);
    });
    basicLayout->addWidget(m_animRigEdit, row, 1);
    m_browseAnimRigBtn = new QPushButton(tr("..."), this);
    m_browseAnimRigBtn->setMaximumWidth(30);
    connect(m_browseAnimRigBtn, &QPushButton::clicked, this, &ModelEditor::onBrowseAnimRig);
    basicLayout->addWidget(m_browseAnimRigBtn, row, 2);
    row++;

    layout->addWidget(basicGroup);

    // Model Info Group (read-only statistics)
    QGroupBox* infoGroup = new QGroupBox(tr("Model Information"), this);
    QGridLayout* infoLayout = new QGridLayout(infoGroup);

    infoLayout->addWidget(new QLabel(tr("Vertices:"), this), 0, 0);
    m_vertexCountLabel = new QLabel("-", this);
    m_vertexCountLabel->setStyleSheet("color: #569cd6;");
    infoLayout->addWidget(m_vertexCountLabel, 0, 1);

    infoLayout->addWidget(new QLabel(tr("Triangles:"), this), 0, 2);
    m_triangleCountLabel = new QLabel("-", this);
    m_triangleCountLabel->setStyleSheet("color: #569cd6;");
    infoLayout->addWidget(m_triangleCountLabel, 0, 3);

    infoLayout->addWidget(new QLabel(tr("LODs:"), this), 1, 0);
    m_lodCountLabel = new QLabel("-", this);
    m_lodCountLabel->setStyleSheet("color: #569cd6;");
    infoLayout->addWidget(m_lodCountLabel, 1, 1);

    infoLayout->addWidget(new QLabel(tr("Submeshes:"), this), 1, 2);
    m_submeshCountLabel = new QLabel("-", this);
    m_submeshCountLabel->setStyleSheet("color: #569cd6;");
    infoLayout->addWidget(m_submeshCountLabel, 1, 3);

    layout->addWidget(infoGroup);

    // Physics Settings Group
    QGroupBox* physicsGroup = new QGroupBox(tr("Physics Settings"), this);
    QGridLayout* physicsLayout = new QGridLayout(physicsGroup);

    physicsLayout->addWidget(new QLabel(tr("Physics Type:"), this), 0, 0);
    m_physicsTypeCombo = new QComboBox(this);
    m_physicsTypeCombo->addItems({
        "None", "Convex Hull", "Bounding Box", "Sphere", "Capsule", "Mesh (Exact)"
    });
    connect(m_physicsTypeCombo, &QComboBox::currentTextChanged, this, [this]() {
        setModified(true);
    });
    physicsLayout->addWidget(m_physicsTypeCombo, 0, 1);

    m_generatePhysicsCheck = new QCheckBox(tr("Auto-generate physics collision"), this);
    connect(m_generatePhysicsCheck, &QCheckBox::toggled, this, [this]() {
        setModified(true);
    });
    physicsLayout->addWidget(m_generatePhysicsCheck, 1, 0, 1, 2);

    layout->addWidget(physicsGroup);

    // LOD Settings Group
    QGroupBox* lodGroup = new QGroupBox(tr("LOD Configuration"), this);
    QVBoxLayout* lodLayout = new QVBoxLayout(lodGroup);

    QHBoxLayout* lodCountLayout = new QHBoxLayout();
    lodCountLayout->addWidget(new QLabel(tr("LOD Levels:"), this));
    m_lodCountSpin = new QSpinBox(this);
    m_lodCountSpin->setRange(1, 8);
    m_lodCountSpin->setValue(1);
    connect(m_lodCountSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
        setModified(true);
    });
    lodCountLayout->addWidget(m_lodCountSpin);
    lodCountLayout->addStretch();
    lodLayout->addLayout(lodCountLayout);

    m_lodTree = new QTreeWidget(this);
    m_lodTree->setHeaderLabels({tr("LOD Level"), tr("Distance"), tr("Triangles")});
    m_lodTree->setMaximumHeight(120);
    m_lodTree->header()->setStretchLastSection(true);
    lodLayout->addWidget(m_lodTree);

    layout->addWidget(lodGroup);

    // Streaming Options Group
    QGroupBox* streamGroup = new QGroupBox(tr("Streaming Options"), this);
    QVBoxLayout* streamLayout = new QVBoxLayout(streamGroup);

    m_streamMandatoryCheck = new QCheckBox(tr("Stream Mandatory (high-resolution data)"), this);
    connect(m_streamMandatoryCheck, &QCheckBox::toggled, this, [this]() {
        setModified(true);
    });
    streamLayout->addWidget(m_streamMandatoryCheck);

    m_streamOptionalCheck = new QCheckBox(tr("Stream Optional (additional detail)"), this);
    connect(m_streamOptionalCheck, &QCheckBox::toggled, this, [this]() {
        setModified(true);
    });
    streamLayout->addWidget(m_streamOptionalCheck);

    layout->addWidget(streamGroup);

    // Material Overrides Group
    QGroupBox* materialsGroup = new QGroupBox(tr("Material Overrides"), this);
    QVBoxLayout* materialsLayout = new QVBoxLayout(materialsGroup);

    m_materialList = new QListWidget(this);
    m_materialList->setMaximumHeight(120);
    connect(m_materialList, &QListWidget::currentRowChanged, this, &ModelEditor::onMaterialSelected);
    materialsLayout->addWidget(m_materialList);

    QHBoxLayout* materialButtons = new QHBoxLayout();
    m_addMaterialBtn = new QPushButton(tr("Add Override"), this);
    connect(m_addMaterialBtn, &QPushButton::clicked, this, &ModelEditor::onAddMaterial);
    m_removeMaterialBtn = new QPushButton(tr("Remove"), this);
    m_removeMaterialBtn->setEnabled(false);
    connect(m_removeMaterialBtn, &QPushButton::clicked, this, &ModelEditor::onRemoveMaterial);
    m_browseMaterialBtn = new QPushButton(tr("Browse..."), this);
    m_browseMaterialBtn->setEnabled(false);
    connect(m_browseMaterialBtn, &QPushButton::clicked, this, &ModelEditor::onBrowseMaterial);
    materialButtons->addWidget(m_addMaterialBtn);
    materialButtons->addWidget(m_removeMaterialBtn);
    materialButtons->addWidget(m_browseMaterialBtn);
    materialButtons->addStretch();
    materialsLayout->addLayout(materialButtons);

    QHBoxLayout* materialPathLayout = new QHBoxLayout();
    materialPathLayout->addWidget(new QLabel(tr("Replacement:"), this));
    m_materialPathEdit = new QLineEdit(this);
    m_materialPathEdit->setPlaceholderText(tr("Select a material override above"));
    m_materialPathEdit->setEnabled(false);
    connect(m_materialPathEdit, &QLineEdit::textChanged, this, [this]() {
        int row = m_materialList->currentRow();
        if (row >= 0 && row < m_materials.size()) {
            m_materials[row].replacementPath = m_materialPathEdit->text();
            updateMaterialList();
            setModified(true);
        }
    });
    materialPathLayout->addWidget(m_materialPathEdit);
    materialsLayout->addLayout(materialPathLayout);

    layout->addWidget(materialsGroup);
    layout->addStretch();

    scrollArea->setWidget(scrollContent);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scrollArea);
}

void ModelEditor::setAsset(const Asset& asset, Project* project)
{
    AssetEditorBase::setAsset(asset, project);

    m_pathEdit->setText(asset.path());
    m_guidEdit->setText(asset.guid());

    const QVariantMap& metadata = asset.metadata();

    // Animation rig
    m_animRigEdit->setText(metadata.value("animRig", "").toString());

    // Model info (would be read from actual file in full implementation)
    m_vertexCountLabel->setText(metadata.value("vertexCount", "-").toString());
    m_triangleCountLabel->setText(metadata.value("triangleCount", "-").toString());
    m_lodCountLabel->setText(metadata.value("lodCount", "-").toString());
    m_submeshCountLabel->setText(metadata.value("submeshCount", "-").toString());

    // Physics
    QString physicsType = metadata.value("physicsType", "None").toString();
    int physicsIdx = m_physicsTypeCombo->findText(physicsType);
    if (physicsIdx >= 0) {
        m_physicsTypeCombo->setCurrentIndex(physicsIdx);
    }
    m_generatePhysicsCheck->setChecked(metadata.value("generatePhysics", false).toBool());

    // LOD
    m_lodCountSpin->setValue(metadata.value("lodLevels", 1).toInt());

    // Update LOD tree
    m_lodTree->clear();
    QVariantList lodList = metadata.value("lods").toList();
    for (int i = 0; i < lodList.size(); ++i) {
        QVariantMap lod = lodList[i].toMap();
        QTreeWidgetItem* item = new QTreeWidgetItem(m_lodTree);
        item->setText(0, QString("LOD %1").arg(i));
        item->setText(1, QString("%1m").arg(lod.value("distance", 0).toDouble(), 0, 'f', 1));
        item->setText(2, lod.value("triangles", "-").toString());
    }

    // Streaming
    m_streamMandatoryCheck->setChecked(metadata.value("streamMandatory", false).toBool());
    m_streamOptionalCheck->setChecked(metadata.value("streamOptional", false).toBool());

    // Material overrides
    m_materials.clear();
    QVariantList materialList = metadata.value("materialOverrides").toList();
    for (const QVariant& mat : materialList) {
        QVariantMap matMap = mat.toMap();
        MaterialOverride override;
        override.originalName = matMap.value("original").toString();
        override.replacementPath = matMap.value("replacement").toString();
        m_materials.append(override);
    }
    updateMaterialList();

    setModified(false);
}

void ModelEditor::clearAsset()
{
    AssetEditorBase::clearAsset();

    m_pathEdit->clear();
    m_guidEdit->clear();
    m_animRigEdit->clear();

    m_vertexCountLabel->setText("-");
    m_triangleCountLabel->setText("-");
    m_lodCountLabel->setText("-");
    m_submeshCountLabel->setText("-");

    m_physicsTypeCombo->setCurrentIndex(0);
    m_generatePhysicsCheck->setChecked(false);

    m_lodCountSpin->setValue(1);
    m_lodTree->clear();

    m_streamMandatoryCheck->setChecked(false);
    m_streamOptionalCheck->setChecked(false);

    m_materials.clear();
    updateMaterialList();
}

void ModelEditor::onBrowseAnimRig()
{
    QString startDir;
    if (m_project) {
        startDir = m_project->absoluteAssetsDir();
    }

    QString path = QFileDialog::getOpenFileName(
        this,
        tr("Select Animation Rig"),
        startDir,
        tr("Animation Rig Files (*.rrig);;All Files (*)")
    );

    if (!path.isEmpty()) {
        if (m_project && !m_project->assetsDir().isEmpty()) {
            QString assetsDir = m_project->absoluteAssetsDir();
            if (path.startsWith(assetsDir)) {
                path = path.mid(assetsDir.length());
                if (path.startsWith("/") || path.startsWith("\\")) {
                    path = path.mid(1);
                }
            }
        }
        m_animRigEdit->setText(path);
        setModified(true);
    }
}

void ModelEditor::onAddMaterial()
{
    MaterialOverride override;
    override.originalName = QString("material_%1").arg(m_materials.size());
    override.replacementPath = "";
    m_materials.append(override);
    updateMaterialList();
    m_materialList->setCurrentRow(m_materials.size() - 1);
    setModified(true);
}

void ModelEditor::onRemoveMaterial()
{
    int row = m_materialList->currentRow();
    if (row >= 0 && row < m_materials.size()) {
        m_materials.removeAt(row);
        updateMaterialList();
        setModified(true);
    }
}

void ModelEditor::onBrowseMaterial()
{
    int row = m_materialList->currentRow();
    if (row < 0 || row >= m_materials.size()) {
        return;
    }

    QString startDir;
    if (m_project) {
        startDir = m_project->absoluteAssetsDir();
    }

    QString path = QFileDialog::getOpenFileName(
        this,
        tr("Select Material"),
        startDir,
        tr("Material Files (*.rpak);;All Files (*)")
    );

    if (!path.isEmpty()) {
        if (m_project && !m_project->assetsDir().isEmpty()) {
            QString assetsDir = m_project->absoluteAssetsDir();
            if (path.startsWith(assetsDir)) {
                path = path.mid(assetsDir.length());
                if (path.startsWith("/") || path.startsWith("\\")) {
                    path = path.mid(1);
                }
            }
        }
        m_materials[row].replacementPath = path;
        m_materialPathEdit->setText(path);
        updateMaterialList();
        setModified(true);
    }
}

void ModelEditor::onMaterialSelected(int row)
{
    bool hasSelection = (row >= 0 && row < m_materials.size());

    m_removeMaterialBtn->setEnabled(hasSelection);
    m_browseMaterialBtn->setEnabled(hasSelection);
    m_materialPathEdit->setEnabled(hasSelection);

    if (hasSelection) {
        const MaterialOverride& override = m_materials[row];
        m_materialPathEdit->blockSignals(true);
        m_materialPathEdit->setText(override.replacementPath);
        m_materialPathEdit->blockSignals(false);
    } else {
        m_materialPathEdit->clear();
    }
}

void ModelEditor::updateMaterialList()
{
    int currentRow = m_materialList->currentRow();
    m_materialList->clear();

    for (const MaterialOverride& override : m_materials) {
        QString display = QString("%1 → %2").arg(override.originalName).arg(
            override.replacementPath.isEmpty() ? tr("(not set)") : override.replacementPath
        );
        m_materialList->addItem(display);
    }

    if (currentRow >= 0 && currentRow < m_materials.size()) {
        m_materialList->setCurrentRow(currentRow);
    }
}
