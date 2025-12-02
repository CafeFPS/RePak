#include "materialeditor.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QScrollArea>

#include "core/project.h"

MaterialEditor::MaterialEditor(QWidget* parent)
    : AssetEditorBase(parent)
{
    setupUi();
}

MaterialEditor::~MaterialEditor()
{
}

void MaterialEditor::setupUi()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* scrollContent = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(scrollContent);

    // Title
    QLabel* titleLabel = new QLabel(tr("Material Asset"), this);
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

    basicLayout->addWidget(new QLabel(tr("Surface Type:"), this), row, 0);
    m_surfaceTypeCombo = new QComboBox(this);
    m_surfaceTypeCombo->addItems({
        "default", "concrete", "metal", "dirt", "grass", "gravel",
        "mud", "sand", "rock", "water", "wood", "glass", "flesh",
        "plastic", "rubber", "cloth", "foliage", "ice", "snow"
    });
    connect(m_surfaceTypeCombo, &QComboBox::currentTextChanged, this, [this]() {
        setModified(true);
    });
    basicLayout->addWidget(m_surfaceTypeCombo, row, 1, 1, 2);
    row++;

    basicLayout->addWidget(new QLabel(tr("Shader:"), this), row, 0);
    m_shaderCombo = new QComboBox(this);
    m_shaderCombo->addItems({
        "general", "general_emissive", "skydome", "water", "skin",
        "hair", "eye", "teeth", "decal", "particle", "cable",
        "unlit", "viewmodel", "scope"
    });
    connect(m_shaderCombo, &QComboBox::currentTextChanged, this, [this]() {
        setModified(true);
    });
    basicLayout->addWidget(m_shaderCombo, row, 1, 1, 2);
    row++;

    layout->addWidget(basicGroup);

    // Material Flags Group
    QGroupBox* flagsGroup = new QGroupBox(tr("Material Flags"), this);
    QGridLayout* flagsLayout = new QGridLayout(flagsGroup);

    m_alphaBlendCheck = new QCheckBox(tr("Alpha Blend"), this);
    connect(m_alphaBlendCheck, &QCheckBox::toggled, this, [this]() { setModified(true); });
    flagsLayout->addWidget(m_alphaBlendCheck, 0, 0);

    m_noShadowCheck = new QCheckBox(tr("No Shadow"), this);
    connect(m_noShadowCheck, &QCheckBox::toggled, this, [this]() { setModified(true); });
    flagsLayout->addWidget(m_noShadowCheck, 0, 1);

    m_doubleSidedCheck = new QCheckBox(tr("Double Sided"), this);
    connect(m_doubleSidedCheck, &QCheckBox::toggled, this, [this]() { setModified(true); });
    flagsLayout->addWidget(m_doubleSidedCheck, 1, 0);

    m_emissiveCheck = new QCheckBox(tr("Emissive"), this);
    connect(m_emissiveCheck, &QCheckBox::toggled, this, [this]() { setModified(true); });
    flagsLayout->addWidget(m_emissiveCheck, 1, 1);

    layout->addWidget(flagsGroup);

    // Material Parameters Group
    QGroupBox* paramsGroup = new QGroupBox(tr("Material Parameters"), this);
    QGridLayout* paramsLayout = new QGridLayout(paramsGroup);
    row = 0;

    paramsLayout->addWidget(new QLabel(tr("Albedo Tint:"), this), row, 0);
    QHBoxLayout* tintLayout = new QHBoxLayout();
    m_albedoTintR = new QDoubleSpinBox(this);
    m_albedoTintR->setRange(0.0, 1.0);
    m_albedoTintR->setSingleStep(0.01);
    m_albedoTintR->setValue(1.0);
    m_albedoTintR->setPrefix("R: ");
    connect(m_albedoTintR, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() { setModified(true); });

    m_albedoTintG = new QDoubleSpinBox(this);
    m_albedoTintG->setRange(0.0, 1.0);
    m_albedoTintG->setSingleStep(0.01);
    m_albedoTintG->setValue(1.0);
    m_albedoTintG->setPrefix("G: ");
    connect(m_albedoTintG, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() { setModified(true); });

    m_albedoTintB = new QDoubleSpinBox(this);
    m_albedoTintB->setRange(0.0, 1.0);
    m_albedoTintB->setSingleStep(0.01);
    m_albedoTintB->setValue(1.0);
    m_albedoTintB->setPrefix("B: ");
    connect(m_albedoTintB, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() { setModified(true); });

    tintLayout->addWidget(m_albedoTintR);
    tintLayout->addWidget(m_albedoTintG);
    tintLayout->addWidget(m_albedoTintB);
    paramsLayout->addLayout(tintLayout, row, 1);
    row++;

    paramsLayout->addWidget(new QLabel(tr("Emissive Multiplier:"), this), row, 0);
    m_emissiveMultiplier = new QDoubleSpinBox(this);
    m_emissiveMultiplier->setRange(0.0, 1000.0);
    m_emissiveMultiplier->setSingleStep(0.1);
    m_emissiveMultiplier->setValue(1.0);
    connect(m_emissiveMultiplier, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() { setModified(true); });
    paramsLayout->addWidget(m_emissiveMultiplier, row, 1);
    row++;

    paramsLayout->addWidget(new QLabel(tr("Glossiness:"), this), row, 0);
    m_glossiness = new QDoubleSpinBox(this);
    m_glossiness->setRange(0.0, 1.0);
    m_glossiness->setSingleStep(0.01);
    m_glossiness->setValue(0.5);
    connect(m_glossiness, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() { setModified(true); });
    paramsLayout->addWidget(m_glossiness, row, 1);

    layout->addWidget(paramsGroup);

    // Texture Slots Group
    QGroupBox* texturesGroup = new QGroupBox(tr("Texture Slots"), this);
    QVBoxLayout* texturesLayout = new QVBoxLayout(texturesGroup);

    m_textureList = new QListWidget(this);
    m_textureList->setMaximumHeight(150);
    connect(m_textureList, &QListWidget::currentRowChanged, this, &MaterialEditor::onTextureSelected);
    texturesLayout->addWidget(m_textureList);

    QHBoxLayout* textureButtons = new QHBoxLayout();
    m_addTextureBtn = new QPushButton(tr("Add"), this);
    connect(m_addTextureBtn, &QPushButton::clicked, this, &MaterialEditor::onAddTexture);
    m_removeTextureBtn = new QPushButton(tr("Remove"), this);
    connect(m_removeTextureBtn, &QPushButton::clicked, this, &MaterialEditor::onRemoveTexture);
    m_browseTextureBtn = new QPushButton(tr("Browse..."), this);
    connect(m_browseTextureBtn, &QPushButton::clicked, this, &MaterialEditor::onBrowseTexture);
    textureButtons->addWidget(m_addTextureBtn);
    textureButtons->addWidget(m_removeTextureBtn);
    textureButtons->addWidget(m_browseTextureBtn);
    textureButtons->addStretch();
    texturesLayout->addLayout(textureButtons);

    // Texture slot properties
    QGridLayout* texturePropsLayout = new QGridLayout();
    texturePropsLayout->addWidget(new QLabel(tr("Texture Path:"), this), 0, 0);
    m_texturePathEdit = new QLineEdit(this);
    m_texturePathEdit->setPlaceholderText(tr("Select a texture slot above"));
    connect(m_texturePathEdit, &QLineEdit::textChanged, this, [this]() {
        int row = m_textureList->currentRow();
        if (row >= 0 && row < m_textures.size()) {
            m_textures[row].path = m_texturePathEdit->text();
            updateTextureList();
            setModified(true);
        }
    });
    texturePropsLayout->addWidget(m_texturePathEdit, 0, 1);

    texturePropsLayout->addWidget(new QLabel(tr("Slot Type:"), this), 1, 0);
    m_textureSlotCombo = new QComboBox(this);
    m_textureSlotCombo->addItems({
        "col (Albedo/Color)", "nml (Normal Map)", "gls (Gloss/Specular)",
        "spc (Specular)", "ilm (Illumination)", "ao (Ambient Occlusion)",
        "cav (Cavity)", "opa (Opacity)", "det (Detail)", "exp (Exponent)",
        "msk (Mask)", "dif (Diffuse)"
    });
    connect(m_textureSlotCombo, &QComboBox::currentTextChanged, this, [this]() {
        int row = m_textureList->currentRow();
        if (row >= 0 && row < m_textures.size()) {
            QString slotType = m_textureSlotCombo->currentText();
            m_textures[row].slotType = slotType.split(" ").first();
            updateTextureList();
            setModified(true);
        }
    });
    texturePropsLayout->addWidget(m_textureSlotCombo, 1, 1);
    texturesLayout->addLayout(texturePropsLayout);

    layout->addWidget(texturesGroup);
    layout->addStretch();

    scrollArea->setWidget(scrollContent);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scrollArea);

    // Initial state
    m_removeTextureBtn->setEnabled(false);
    m_browseTextureBtn->setEnabled(false);
    m_texturePathEdit->setEnabled(false);
    m_textureSlotCombo->setEnabled(false);
}

void MaterialEditor::setAsset(const Asset& asset, Project* project)
{
    AssetEditorBase::setAsset(asset, project);

    m_pathEdit->setText(asset.path());
    m_guidEdit->setText(asset.guid());

    // Load material-specific properties from asset metadata
    const QVariantMap& metadata = asset.metadata();

    // Surface type
    QString surfaceType = metadata.value("surfaceType", "default").toString();
    int surfaceIdx = m_surfaceTypeCombo->findText(surfaceType);
    if (surfaceIdx >= 0) {
        m_surfaceTypeCombo->setCurrentIndex(surfaceIdx);
    }

    // Shader
    QString shader = metadata.value("shader", "general").toString();
    int shaderIdx = m_shaderCombo->findText(shader);
    if (shaderIdx >= 0) {
        m_shaderCombo->setCurrentIndex(shaderIdx);
    }

    // Flags
    m_alphaBlendCheck->setChecked(metadata.value("alphaBlend", false).toBool());
    m_noShadowCheck->setChecked(metadata.value("noShadow", false).toBool());
    m_doubleSidedCheck->setChecked(metadata.value("doubleSided", false).toBool());
    m_emissiveCheck->setChecked(metadata.value("emissive", false).toBool());

    // Parameters
    m_albedoTintR->setValue(metadata.value("albedoTintR", 1.0).toDouble());
    m_albedoTintG->setValue(metadata.value("albedoTintG", 1.0).toDouble());
    m_albedoTintB->setValue(metadata.value("albedoTintB", 1.0).toDouble());
    m_emissiveMultiplier->setValue(metadata.value("emissiveMultiplier", 1.0).toDouble());
    m_glossiness->setValue(metadata.value("glossiness", 0.5).toDouble());

    // Textures
    m_textures.clear();
    QVariantList textureList = metadata.value("textures").toList();
    for (const QVariant& tex : textureList) {
        QVariantMap texMap = tex.toMap();
        TextureSlot slot;
        slot.path = texMap.value("path").toString();
        slot.slotType = texMap.value("slot", "col").toString();
        m_textures.append(slot);
    }
    updateTextureList();

    setModified(false);
}

void MaterialEditor::clearAsset()
{
    AssetEditorBase::clearAsset();

    m_pathEdit->clear();
    m_guidEdit->clear();
    m_surfaceTypeCombo->setCurrentIndex(0);
    m_shaderCombo->setCurrentIndex(0);

    m_alphaBlendCheck->setChecked(false);
    m_noShadowCheck->setChecked(false);
    m_doubleSidedCheck->setChecked(false);
    m_emissiveCheck->setChecked(false);

    m_albedoTintR->setValue(1.0);
    m_albedoTintG->setValue(1.0);
    m_albedoTintB->setValue(1.0);
    m_emissiveMultiplier->setValue(1.0);
    m_glossiness->setValue(0.5);

    m_textures.clear();
    updateTextureList();
}

void MaterialEditor::onAddTexture()
{
    TextureSlot slot;
    slot.path = "";
    slot.slotType = "col";
    m_textures.append(slot);
    updateTextureList();
    m_textureList->setCurrentRow(m_textures.size() - 1);
    setModified(true);
}

void MaterialEditor::onRemoveTexture()
{
    int row = m_textureList->currentRow();
    if (row >= 0 && row < m_textures.size()) {
        m_textures.removeAt(row);
        updateTextureList();
        setModified(true);
    }
}

void MaterialEditor::onBrowseTexture()
{
    int row = m_textureList->currentRow();
    if (row < 0 || row >= m_textures.size()) {
        return;
    }

    QString startDir;
    if (m_project) {
        startDir = m_project->absoluteAssetsDir();
    }

    QString path = QFileDialog::getOpenFileName(
        this,
        tr("Select Texture"),
        startDir,
        tr("Texture Files (*.dds *.png *.tga);;All Files (*)")
    );

    if (!path.isEmpty()) {
        // Make relative to assets dir if possible
        if (m_project && !m_project->assetsDir().isEmpty()) {
            QString assetsDir = m_project->absoluteAssetsDir();
            if (path.startsWith(assetsDir)) {
                path = path.mid(assetsDir.length());
                if (path.startsWith("/") || path.startsWith("\\")) {
                    path = path.mid(1);
                }
            }
        }
        m_textures[row].path = path;
        m_texturePathEdit->setText(path);
        updateTextureList();
        setModified(true);
    }
}

void MaterialEditor::onTextureSelected(int row)
{
    bool hasSelection = (row >= 0 && row < m_textures.size());

    m_removeTextureBtn->setEnabled(hasSelection);
    m_browseTextureBtn->setEnabled(hasSelection);
    m_texturePathEdit->setEnabled(hasSelection);
    m_textureSlotCombo->setEnabled(hasSelection);

    if (hasSelection) {
        const TextureSlot& slot = m_textures[row];
        m_texturePathEdit->blockSignals(true);
        m_texturePathEdit->setText(slot.path);
        m_texturePathEdit->blockSignals(false);

        m_textureSlotCombo->blockSignals(true);
        for (int i = 0; i < m_textureSlotCombo->count(); ++i) {
            if (m_textureSlotCombo->itemText(i).startsWith(slot.slotType)) {
                m_textureSlotCombo->setCurrentIndex(i);
                break;
            }
        }
        m_textureSlotCombo->blockSignals(false);
    } else {
        m_texturePathEdit->clear();
        m_textureSlotCombo->setCurrentIndex(0);
    }
}

void MaterialEditor::updateTextureList()
{
    int currentRow = m_textureList->currentRow();
    m_textureList->clear();

    for (const TextureSlot& slot : m_textures) {
        QString display = QString("[%1] %2").arg(slot.slotType).arg(
            slot.path.isEmpty() ? tr("(empty)") : slot.path
        );
        m_textureList->addItem(display);
    }

    if (currentRow >= 0 && currentRow < m_textures.size()) {
        m_textureList->setCurrentRow(currentRow);
    }
}
