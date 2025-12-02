#include "textureeditor.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>

TextureEditor::TextureEditor(QWidget* parent)
    : AssetEditorBase(parent)
{
    setupUi();
}

TextureEditor::~TextureEditor()
{
}

void TextureEditor::setupUi()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    // Title
    QLabel* titleLabel = new QLabel(tr("Texture Asset"), this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    layout->addWidget(titleLabel);

    // Preview
    QGroupBox* previewGroup = new QGroupBox(tr("Preview"), this);
    QVBoxLayout* previewLayout = new QVBoxLayout(previewGroup);

    m_previewLabel = new QLabel(this);
    m_previewLabel->setMinimumSize(256, 256);
    m_previewLabel->setMaximumSize(256, 256);
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setStyleSheet("background-color: #2d2d2d; border: 1px solid #3c3c3c;");
    m_previewLabel->setText(tr("No Preview"));
    previewLayout->addWidget(m_previewLabel, 0, Qt::AlignCenter);

    layout->addWidget(previewGroup);

    // Properties
    QGroupBox* propsGroup = new QGroupBox(tr("Properties"), this);
    QGridLayout* propsLayout = new QGridLayout(propsGroup);

    int row = 0;

    propsLayout->addWidget(new QLabel(tr("Path:"), this), row, 0);
    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setReadOnly(true);
    propsLayout->addWidget(m_pathEdit, row, 1);
    row++;

    propsLayout->addWidget(new QLabel(tr("GUID:"), this), row, 0);
    m_guidEdit = new QLineEdit(this);
    connect(m_guidEdit, &QLineEdit::textChanged, this, [this]() {
        setModified(true);
    });
    propsLayout->addWidget(m_guidEdit, row, 1);
    row++;

    propsLayout->addWidget(new QLabel(tr("Format:"), this), row, 0);
    m_formatCombo = new QComboBox(this);
    m_formatCombo->addItems({"DXT1/BC1", "DXT3/BC2", "DXT5/BC3", "BC4", "BC5", "BC7", "RGBA8"});
    propsLayout->addWidget(m_formatCombo, row, 1);
    row++;

    propsLayout->addWidget(new QLabel(tr("Dimensions:"), this), row, 0);
    QHBoxLayout* dimLayout = new QHBoxLayout();
    m_widthEdit = new QLineEdit(this);
    m_widthEdit->setReadOnly(true);
    m_widthEdit->setMaximumWidth(80);
    m_heightEdit = new QLineEdit(this);
    m_heightEdit->setReadOnly(true);
    m_heightEdit->setMaximumWidth(80);
    dimLayout->addWidget(m_widthEdit);
    dimLayout->addWidget(new QLabel("x", this));
    dimLayout->addWidget(m_heightEdit);
    dimLayout->addStretch();
    propsLayout->addLayout(dimLayout, row, 1);

    layout->addWidget(propsGroup);
    layout->addStretch();
}

void TextureEditor::setAsset(const Asset& asset, Project* project)
{
    AssetEditorBase::setAsset(asset, project);

    m_pathEdit->setText(asset.path());
    m_guidEdit->setText(asset.guid());

    // TODO: Load actual texture info
    m_widthEdit->setText("-");
    m_heightEdit->setText("-");

    updatePreview();
}

void TextureEditor::clearAsset()
{
    AssetEditorBase::clearAsset();

    m_pathEdit->clear();
    m_guidEdit->clear();
    m_widthEdit->clear();
    m_heightEdit->clear();
    m_previewLabel->setText(tr("No Preview"));
    m_previewLabel->setPixmap(QPixmap());
}

void TextureEditor::updatePreview()
{
    // TODO: Load and display texture preview
    m_previewLabel->setText(tr("Preview not available"));
}
