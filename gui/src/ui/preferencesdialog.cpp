#include "preferencesdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QDialogButtonBox>

#include "core/settings.h"

PreferencesDialog::PreferencesDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
    loadSettings();
}

PreferencesDialog::~PreferencesDialog()
{
}

void PreferencesDialog::setupUi()
{
    setWindowTitle(tr("Preferences"));
    setMinimumSize(550, 450);

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_tabWidget = new QTabWidget(this);

    // === Paths Tab ===
    QWidget* pathsTab = new QWidget();
    QVBoxLayout* pathsLayout = new QVBoxLayout(pathsTab);

    QGroupBox* pathsGroup = new QGroupBox(tr("Paths"), pathsTab);
    QGridLayout* pathsGrid = new QGridLayout(pathsGroup);

    pathsGrid->addWidget(new QLabel(tr("RePak Executable:"), this), 0, 0);
    m_repakPathEdit = new QLineEdit(this);
    pathsGrid->addWidget(m_repakPathEdit, 0, 1);
    QPushButton* repakBrowse = new QPushButton(tr("Browse..."), this);
    connect(repakBrowse, &QPushButton::clicked, this, &PreferencesDialog::browseRepakPath);
    pathsGrid->addWidget(repakBrowse, 0, 2);

    pathsGrid->addWidget(new QLabel(tr("Game Directory:"), this), 1, 0);
    m_gameDirEdit = new QLineEdit(this);
    pathsGrid->addWidget(m_gameDirEdit, 1, 1);
    QPushButton* gameBrowse = new QPushButton(tr("Browse..."), this);
    connect(gameBrowse, &QPushButton::clicked, this, &PreferencesDialog::browseGameDir);
    pathsGrid->addWidget(gameBrowse, 1, 2);

    pathsGrid->addWidget(new QLabel(tr("Default Projects Dir:"), this), 2, 0);
    m_projectsDirEdit = new QLineEdit(this);
    pathsGrid->addWidget(m_projectsDirEdit, 2, 1);
    QPushButton* projectsBrowse = new QPushButton(tr("Browse..."), this);
    connect(projectsBrowse, &QPushButton::clicked, this, &PreferencesDialog::browseProjectsDir);
    pathsGrid->addWidget(projectsBrowse, 2, 2);

    pathsLayout->addWidget(pathsGroup);
    pathsLayout->addStretch();
    m_tabWidget->addTab(pathsTab, tr("Paths"));

    // === Build Tab ===
    QWidget* buildTab = new QWidget();
    QVBoxLayout* buildLayout = new QVBoxLayout(buildTab);

    QGroupBox* compressGroup = new QGroupBox(tr("Compression"), buildTab);
    QGridLayout* compressGrid = new QGridLayout(compressGroup);

    compressGrid->addWidget(new QLabel(tr("Default Level:"), this), 0, 0);
    m_compressLevelSpin = new QSpinBox(this);
    m_compressLevelSpin->setRange(-5, 22);
    compressGrid->addWidget(m_compressLevelSpin, 0, 1);

    compressGrid->addWidget(new QLabel(tr("Worker Threads:"), this), 1, 0);
    m_compressWorkersSpin = new QSpinBox(this);
    m_compressWorkersSpin->setRange(1, 64);
    compressGrid->addWidget(m_compressWorkersSpin, 1, 1);

    m_autoCompressCheck = new QCheckBox(tr("Auto-compress builds"), this);
    compressGrid->addWidget(m_autoCompressCheck, 2, 0, 1, 2);

    m_copyToGameCheck = new QCheckBox(tr("Copy PAK to game directory after build"), this);
    compressGrid->addWidget(m_copyToGameCheck, 3, 0, 1, 2);

    buildLayout->addWidget(compressGroup);
    buildLayout->addStretch();
    m_tabWidget->addTab(buildTab, tr("Build"));

    // === Editor Tab ===
    QWidget* editorTab = new QWidget();
    QVBoxLayout* editorLayout = new QVBoxLayout(editorTab);

    QGroupBox* previewGroup = new QGroupBox(tr("Previews"), editorTab);
    QGridLayout* previewGrid = new QGridLayout(previewGroup);

    previewGrid->addWidget(new QLabel(tr("Texture Preview Size:"), this), 0, 0);
    m_previewSizeSpin = new QSpinBox(this);
    m_previewSizeSpin->setRange(64, 512);
    m_previewSizeSpin->setSingleStep(64);
    m_previewSizeSpin->setSuffix(" px");
    previewGrid->addWidget(m_previewSizeSpin, 0, 1);

    m_showPreviewsCheck = new QCheckBox(tr("Show asset previews in browser"), this);
    previewGrid->addWidget(m_showPreviewsCheck, 1, 0, 1, 2);

    m_enable3DCheck = new QCheckBox(tr("Enable 3D model preview"), this);
    previewGrid->addWidget(m_enable3DCheck, 2, 0, 1, 2);

    editorLayout->addWidget(previewGroup);
    editorLayout->addStretch();
    m_tabWidget->addTab(editorTab, tr("Editor"));

    // === General Tab ===
    QWidget* generalTab = new QWidget();
    QVBoxLayout* generalLayout = new QVBoxLayout(generalTab);

    QGroupBox* uiGroup = new QGroupBox(tr("User Interface"), generalTab);
    QVBoxLayout* uiLayout = new QVBoxLayout(uiGroup);

    m_darkThemeCheck = new QCheckBox(tr("Use dark theme"), this);
    uiLayout->addWidget(m_darkThemeCheck);

    m_confirmDialogsCheck = new QCheckBox(tr("Show confirmation dialogs"), this);
    uiLayout->addWidget(m_confirmDialogsCheck);

    QHBoxLayout* autoSaveLayout = new QHBoxLayout();
    autoSaveLayout->addWidget(new QLabel(tr("Auto-save interval:"), this));
    m_autoSaveSpin = new QSpinBox(this);
    m_autoSaveSpin->setRange(1, 60);
    m_autoSaveSpin->setSuffix(tr(" min"));
    autoSaveLayout->addWidget(m_autoSaveSpin);
    autoSaveLayout->addStretch();
    uiLayout->addLayout(autoSaveLayout);

    m_rememberLastCheck = new QCheckBox(tr("Remember last opened project"), this);
    uiLayout->addWidget(m_rememberLastCheck);

    generalLayout->addWidget(uiGroup);
    generalLayout->addStretch();
    m_tabWidget->addTab(generalTab, tr("General"));

    layout->addWidget(m_tabWidget);

    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    buttonBox->addButton(QDialogButtonBox::Apply);
    buttonBox->addButton(QDialogButtonBox::RestoreDefaults);

    connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, [this]() {
        apply();
        accept();
    });
    connect(buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &QDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &PreferencesDialog::apply);
    connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &PreferencesDialog::reset);

    layout->addWidget(buttonBox);
}

void PreferencesDialog::loadSettings()
{
    Settings& s = Settings::instance();

    m_repakPathEdit->setText(s.repakPath());
    m_gameDirEdit->setText(s.gameDirectory());
    m_projectsDirEdit->setText(s.defaultProjectsDir());

    m_compressLevelSpin->setValue(s.defaultCompressLevel());
    m_compressWorkersSpin->setValue(s.defaultCompressWorkers());
    m_autoCompressCheck->setChecked(s.autoCompressBuilds());
    m_copyToGameCheck->setChecked(s.copyToGameAfterBuild());

    m_previewSizeSpin->setValue(s.texturePreviewSize());
    m_showPreviewsCheck->setChecked(s.showAssetPreviews());
    m_enable3DCheck->setChecked(s.enable3DPreview());

    m_darkThemeCheck->setChecked(s.darkTheme());
    m_confirmDialogsCheck->setChecked(s.confirmDialogs());
    m_autoSaveSpin->setValue(s.autoSaveInterval());
    m_rememberLastCheck->setChecked(s.rememberLastProject());
}

void PreferencesDialog::saveSettings()
{
    Settings& s = Settings::instance();

    s.setRepakPath(m_repakPathEdit->text());
    s.setGameDirectory(m_gameDirEdit->text());
    s.setDefaultProjectsDir(m_projectsDirEdit->text());

    s.setDefaultCompressLevel(m_compressLevelSpin->value());
    s.setDefaultCompressWorkers(m_compressWorkersSpin->value());
    s.setAutoCompressBuilds(m_autoCompressCheck->isChecked());
    s.setCopyToGameAfterBuild(m_copyToGameCheck->isChecked());

    s.setTexturePreviewSize(m_previewSizeSpin->value());
    s.setShowAssetPreviews(m_showPreviewsCheck->isChecked());
    s.setEnable3DPreview(m_enable3DCheck->isChecked());

    s.setDarkTheme(m_darkThemeCheck->isChecked());
    s.setConfirmDialogs(m_confirmDialogsCheck->isChecked());
    s.setAutoSaveInterval(m_autoSaveSpin->value());
    s.setRememberLastProject(m_rememberLastCheck->isChecked());

    s.save();
}

void PreferencesDialog::browseRepakPath()
{
    QString path = QFileDialog::getOpenFileName(
        this,
        tr("Select RePak Executable"),
        m_repakPathEdit->text(),
        tr("Executable (*.exe);;All Files (*)")
    );
    if (!path.isEmpty()) {
        m_repakPathEdit->setText(path);
    }
}

void PreferencesDialog::browseGameDir()
{
    QString path = QFileDialog::getExistingDirectory(
        this,
        tr("Select Game Directory"),
        m_gameDirEdit->text()
    );
    if (!path.isEmpty()) {
        m_gameDirEdit->setText(path);
    }
}

void PreferencesDialog::browseProjectsDir()
{
    QString path = QFileDialog::getExistingDirectory(
        this,
        tr("Select Projects Directory"),
        m_projectsDirEdit->text()
    );
    if (!path.isEmpty()) {
        m_projectsDirEdit->setText(path);
    }
}

void PreferencesDialog::apply()
{
    saveSettings();
}

void PreferencesDialog::reset()
{
    Settings::instance().reset();
    loadSettings();
}
