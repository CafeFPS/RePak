#include "shadereditor.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QScrollArea>
#include <QFile>
#include <QTextStream>

#include "core/project.h"

ShaderEditor::ShaderEditor(QWidget* parent)
    : AssetEditorBase(parent)
{
    setupUi();
}

ShaderEditor::~ShaderEditor()
{
}

void ShaderEditor::setupUi()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* scrollContent = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(scrollContent);

    // Title
    QLabel* titleLabel = new QLabel(tr("Shader Asset"), this);
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

    basicLayout->addWidget(new QLabel(tr("Shader Type:"), this), row, 0);
    m_shaderTypeCombo = new QComboBox(this);
    m_shaderTypeCombo->addItems({
        "Vertex Shader (vs)", "Pixel Shader (ps)", "Compute Shader (cs)",
        "Geometry Shader (gs)", "Hull Shader (hs)", "Domain Shader (ds)"
    });
    connect(m_shaderTypeCombo, &QComboBox::currentTextChanged, this, [this]() {
        setModified(true);
    });
    basicLayout->addWidget(m_shaderTypeCombo, row, 1, 1, 2);
    row++;

    basicLayout->addWidget(new QLabel(tr("Shader Model:"), this), row, 0);
    m_shaderModelCombo = new QComboBox(this);
    m_shaderModelCombo->addItems({
        "5_0", "5_1", "6_0", "6_1", "6_2", "6_3", "6_4", "6_5", "6_6"
    });
    m_shaderModelCombo->setCurrentText("5_0");
    connect(m_shaderModelCombo, &QComboBox::currentTextChanged, this, [this]() {
        setModified(true);
    });
    basicLayout->addWidget(m_shaderModelCombo, row, 1, 1, 2);
    row++;

    layout->addWidget(basicGroup);

    // Source File Group
    QGroupBox* sourceGroup = new QGroupBox(tr("Source File"), this);
    QGridLayout* sourceLayout = new QGridLayout(sourceGroup);
    row = 0;

    sourceLayout->addWidget(new QLabel(tr("Source Path:"), this), row, 0);
    m_sourcePathEdit = new QLineEdit(this);
    m_sourcePathEdit->setPlaceholderText(tr("Path to HLSL source file"));
    connect(m_sourcePathEdit, &QLineEdit::textChanged, this, [this]() {
        setModified(true);
    });
    sourceLayout->addWidget(m_sourcePathEdit, row, 1);
    m_browseSourceBtn = new QPushButton(tr("..."), this);
    m_browseSourceBtn->setMaximumWidth(30);
    connect(m_browseSourceBtn, &QPushButton::clicked, this, &ShaderEditor::onBrowseSource);
    sourceLayout->addWidget(m_browseSourceBtn, row, 2);
    row++;

    sourceLayout->addWidget(new QLabel(tr("Entry Point:"), this), row, 0);
    m_entryPointEdit = new QLineEdit(this);
    m_entryPointEdit->setText("main");
    m_entryPointEdit->setPlaceholderText("main");
    connect(m_entryPointEdit, &QLineEdit::textChanged, this, [this]() {
        setModified(true);
    });
    sourceLayout->addWidget(m_entryPointEdit, row, 1, 1, 2);
    row++;

    layout->addWidget(sourceGroup);

    // Compilation Options Group
    QGroupBox* compileGroup = new QGroupBox(tr("Compilation Options"), this);
    QGridLayout* compileLayout = new QGridLayout(compileGroup);

    m_debugInfoCheck = new QCheckBox(tr("Include Debug Information"), this);
    connect(m_debugInfoCheck, &QCheckBox::toggled, this, [this]() {
        setModified(true);
    });
    compileLayout->addWidget(m_debugInfoCheck, 0, 0);

    m_optimizeCheck = new QCheckBox(tr("Enable Optimization"), this);
    m_optimizeCheck->setChecked(true);
    connect(m_optimizeCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_optimizeLevelCombo->setEnabled(checked);
        setModified(true);
    });
    compileLayout->addWidget(m_optimizeCheck, 0, 1);

    compileLayout->addWidget(new QLabel(tr("Optimization Level:"), this), 1, 0);
    m_optimizeLevelCombo = new QComboBox(this);
    m_optimizeLevelCombo->addItems({"O0 (None)", "O1 (Minimal)", "O2 (Default)", "O3 (Full)"});
    m_optimizeLevelCombo->setCurrentIndex(2);
    connect(m_optimizeLevelCombo, &QComboBox::currentTextChanged, this, [this]() {
        setModified(true);
    });
    compileLayout->addWidget(m_optimizeLevelCombo, 1, 1);

    layout->addWidget(compileGroup);

    // Preprocessor Defines Group
    QGroupBox* definesGroup = new QGroupBox(tr("Preprocessor Defines"), this);
    QVBoxLayout* definesLayout = new QVBoxLayout(definesGroup);

    m_definesList = new QListWidget(this);
    m_definesList->setMaximumHeight(100);
    connect(m_definesList, &QListWidget::currentRowChanged, this, &ShaderEditor::onDefineSelected);
    definesLayout->addWidget(m_definesList);

    QHBoxLayout* defineButtons = new QHBoxLayout();
    m_addDefineBtn = new QPushButton(tr("Add"), this);
    connect(m_addDefineBtn, &QPushButton::clicked, this, &ShaderEditor::onAddDefine);
    m_removeDefineBtn = new QPushButton(tr("Remove"), this);
    m_removeDefineBtn->setEnabled(false);
    connect(m_removeDefineBtn, &QPushButton::clicked, this, &ShaderEditor::onRemoveDefine);
    defineButtons->addWidget(m_addDefineBtn);
    defineButtons->addWidget(m_removeDefineBtn);
    defineButtons->addStretch();
    definesLayout->addLayout(defineButtons);

    QGridLayout* definePropsLayout = new QGridLayout();
    definePropsLayout->addWidget(new QLabel(tr("Name:"), this), 0, 0);
    m_defineNameEdit = new QLineEdit(this);
    m_defineNameEdit->setPlaceholderText(tr("DEFINE_NAME"));
    m_defineNameEdit->setEnabled(false);
    connect(m_defineNameEdit, &QLineEdit::textChanged, this, [this]() {
        int row = m_definesList->currentRow();
        if (row >= 0 && row < m_defines.size()) {
            m_defines[row].name = m_defineNameEdit->text();
            updateDefinesList();
            setModified(true);
        }
    });
    definePropsLayout->addWidget(m_defineNameEdit, 0, 1);

    definePropsLayout->addWidget(new QLabel(tr("Value:"), this), 1, 0);
    m_defineValueEdit = new QLineEdit(this);
    m_defineValueEdit->setPlaceholderText(tr("Optional value"));
    m_defineValueEdit->setEnabled(false);
    connect(m_defineValueEdit, &QLineEdit::textChanged, this, [this]() {
        int row = m_definesList->currentRow();
        if (row >= 0 && row < m_defines.size()) {
            m_defines[row].value = m_defineValueEdit->text();
            updateDefinesList();
            setModified(true);
        }
    });
    definePropsLayout->addWidget(m_defineValueEdit, 1, 1);
    definesLayout->addLayout(definePropsLayout);

    layout->addWidget(definesGroup);

    // Code Preview Group
    QGroupBox* previewGroup = new QGroupBox(tr("Code Preview"), this);
    QVBoxLayout* previewLayout = new QVBoxLayout(previewGroup);

    m_codePreview = new QTextEdit(this);
    m_codePreview->setReadOnly(true);
    m_codePreview->setMaximumHeight(200);
    m_codePreview->setStyleSheet(
        "QTextEdit { font-family: 'Consolas', 'Courier New', monospace; "
        "background-color: #1e1e1e; color: #d4d4d4; }"
    );
    m_codePreview->setPlaceholderText(tr("Shader source code will appear here..."));
    previewLayout->addWidget(m_codePreview);

    layout->addWidget(previewGroup);
    layout->addStretch();

    scrollArea->setWidget(scrollContent);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scrollArea);
}

void ShaderEditor::setAsset(const Asset& asset, Project* project)
{
    AssetEditorBase::setAsset(asset, project);

    m_pathEdit->setText(asset.path());
    m_guidEdit->setText(asset.guid());

    const QVariantMap& metadata = asset.metadata();

    // Shader type
    QString shaderType = metadata.value("shaderType", "Vertex Shader (vs)").toString();
    int typeIdx = m_shaderTypeCombo->findText(shaderType, Qt::MatchContains);
    if (typeIdx >= 0) {
        m_shaderTypeCombo->setCurrentIndex(typeIdx);
    }

    // Shader model
    QString shaderModel = metadata.value("shaderModel", "5_0").toString();
    int modelIdx = m_shaderModelCombo->findText(shaderModel);
    if (modelIdx >= 0) {
        m_shaderModelCombo->setCurrentIndex(modelIdx);
    }

    // Source file
    m_sourcePathEdit->setText(metadata.value("sourcePath", "").toString());
    m_entryPointEdit->setText(metadata.value("entryPoint", "main").toString());

    // Compilation options
    m_debugInfoCheck->setChecked(metadata.value("debugInfo", false).toBool());
    m_optimizeCheck->setChecked(metadata.value("optimize", true).toBool());
    int optLevel = metadata.value("optimizeLevel", 2).toInt();
    m_optimizeLevelCombo->setCurrentIndex(qBound(0, optLevel, 3));
    m_optimizeLevelCombo->setEnabled(m_optimizeCheck->isChecked());

    // Defines
    m_defines.clear();
    QVariantList defineList = metadata.value("defines").toList();
    for (const QVariant& def : defineList) {
        QVariantMap defMap = def.toMap();
        ShaderDefine define;
        define.name = defMap.value("name").toString();
        define.value = defMap.value("value").toString();
        m_defines.append(define);
    }
    updateDefinesList();

    // Try to load code preview
    QString sourcePath = m_sourcePathEdit->text();
    if (!sourcePath.isEmpty() && project) {
        QString fullPath = project->absoluteAssetsDir() + "/" + sourcePath;
        QFile file(fullPath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            QString code = stream.readAll();
            if (code.length() > 5000) {
                code = code.left(5000) + "\n\n// ... (truncated)";
            }
            m_codePreview->setPlainText(code);
            file.close();
        } else {
            m_codePreview->setPlainText(tr("// Could not load source file"));
        }
    } else {
        m_codePreview->clear();
    }

    setModified(false);
}

void ShaderEditor::clearAsset()
{
    AssetEditorBase::clearAsset();

    m_pathEdit->clear();
    m_guidEdit->clear();
    m_shaderTypeCombo->setCurrentIndex(0);
    m_shaderModelCombo->setCurrentIndex(0);
    m_sourcePathEdit->clear();
    m_entryPointEdit->setText("main");
    m_debugInfoCheck->setChecked(false);
    m_optimizeCheck->setChecked(true);
    m_optimizeLevelCombo->setCurrentIndex(2);
    m_optimizeLevelCombo->setEnabled(true);
    m_defines.clear();
    updateDefinesList();
    m_codePreview->clear();
}

void ShaderEditor::onAddDefine()
{
    ShaderDefine define;
    define.name = QString("DEFINE_%1").arg(m_defines.size());
    define.value = "";
    m_defines.append(define);
    updateDefinesList();
    m_definesList->setCurrentRow(m_defines.size() - 1);
    setModified(true);
}

void ShaderEditor::onRemoveDefine()
{
    int row = m_definesList->currentRow();
    if (row >= 0 && row < m_defines.size()) {
        m_defines.removeAt(row);
        updateDefinesList();
        setModified(true);
    }
}

void ShaderEditor::onDefineSelected(int row)
{
    bool hasSelection = (row >= 0 && row < m_defines.size());

    m_removeDefineBtn->setEnabled(hasSelection);
    m_defineNameEdit->setEnabled(hasSelection);
    m_defineValueEdit->setEnabled(hasSelection);

    if (hasSelection) {
        const ShaderDefine& define = m_defines[row];
        m_defineNameEdit->blockSignals(true);
        m_defineNameEdit->setText(define.name);
        m_defineNameEdit->blockSignals(false);

        m_defineValueEdit->blockSignals(true);
        m_defineValueEdit->setText(define.value);
        m_defineValueEdit->blockSignals(false);
    } else {
        m_defineNameEdit->clear();
        m_defineValueEdit->clear();
    }
}

void ShaderEditor::onBrowseSource()
{
    QString startDir;
    if (m_project) {
        startDir = m_project->absoluteAssetsDir();
    }

    QString path = QFileDialog::getOpenFileName(
        this,
        tr("Select Shader Source"),
        startDir,
        tr("HLSL Files (*.hlsl *.fx *.fxh);;All Files (*)")
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
        m_sourcePathEdit->setText(path);

        // Try to load preview
        QString fullPath = m_project ? m_project->absoluteAssetsDir() + "/" + path : path;
        QFile file(fullPath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            QString code = stream.readAll();
            if (code.length() > 5000) {
                code = code.left(5000) + "\n\n// ... (truncated)";
            }
            m_codePreview->setPlainText(code);
            file.close();
        }

        setModified(true);
    }
}

void ShaderEditor::updateDefinesList()
{
    int currentRow = m_definesList->currentRow();
    m_definesList->clear();

    for (const ShaderDefine& define : m_defines) {
        QString display;
        if (define.value.isEmpty()) {
            display = define.name;
        } else {
            display = QString("%1 = %2").arg(define.name).arg(define.value);
        }
        m_definesList->addItem(display);
    }

    if (currentRow >= 0 && currentRow < m_defines.size()) {
        m_definesList->setCurrentRow(currentRow);
    }
}
