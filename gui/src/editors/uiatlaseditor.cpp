#include "uiatlaseditor.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QScrollArea>
#include <QHeaderView>

#include "core/project.h"
#include "utils/guidcalculator.h"

UIAtlasEditor::UIAtlasEditor(QWidget* parent)
    : AssetEditorBase(parent)
{
    setupUi();
}

UIAtlasEditor::~UIAtlasEditor()
{
}

void UIAtlasEditor::setupUi()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* scrollContent = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(scrollContent);

    // Title
    QLabel* titleLabel = new QLabel(tr("UI Image Atlas"), this);
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

    basicLayout->addWidget(new QLabel(tr("Atlas Name:"), this), row, 0);
    m_atlasNameEdit = new QLineEdit(this);
    m_atlasNameEdit->setPlaceholderText(tr("e.g., hud_icons"));
    connect(m_atlasNameEdit, &QLineEdit::textChanged, this, [this]() {
        setModified(true);
    });
    basicLayout->addWidget(m_atlasNameEdit, row, 1, 1, 2);
    row++;

    layout->addWidget(basicGroup);

    // Texture Reference Group
    QGroupBox* textureGroup = new QGroupBox(tr("Texture Reference"), this);
    QGridLayout* textureLayout = new QGridLayout(textureGroup);
    row = 0;

    textureLayout->addWidget(new QLabel(tr("Texture:"), this), row, 0);
    m_texturePathEdit = new QLineEdit(this);
    m_texturePathEdit->setPlaceholderText(tr("Path to atlas texture"));
    connect(m_texturePathEdit, &QLineEdit::textChanged, this, [this]() {
        setModified(true);
    });
    textureLayout->addWidget(m_texturePathEdit, row, 1);
    m_browseTextureBtn = new QPushButton(tr("..."), this);
    m_browseTextureBtn->setMaximumWidth(30);
    connect(m_browseTextureBtn, &QPushButton::clicked, this, &UIAtlasEditor::onBrowseTexture);
    textureLayout->addWidget(m_browseTextureBtn, row, 2);
    row++;

    textureLayout->addWidget(new QLabel(tr("Atlas Size:"), this), row, 0);
    QHBoxLayout* sizeLayout = new QHBoxLayout();
    m_atlasWidthSpin = new QSpinBox(this);
    m_atlasWidthSpin->setRange(1, 8192);
    m_atlasWidthSpin->setValue(2048);
    m_atlasWidthSpin->setSuffix(" px");
    connect(m_atlasWidthSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
        setModified(true);
    });
    sizeLayout->addWidget(m_atlasWidthSpin);
    sizeLayout->addWidget(new QLabel("x", this));
    m_atlasHeightSpin = new QSpinBox(this);
    m_atlasHeightSpin->setRange(1, 8192);
    m_atlasHeightSpin->setValue(2048);
    m_atlasHeightSpin->setSuffix(" px");
    connect(m_atlasHeightSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
        setModified(true);
    });
    sizeLayout->addWidget(m_atlasHeightSpin);
    sizeLayout->addStretch();
    textureLayout->addLayout(sizeLayout, row, 1, 1, 2);

    layout->addWidget(textureGroup);

    // Image List Group
    QGroupBox* imagesGroup = new QGroupBox(tr("Images in Atlas"), this);
    QVBoxLayout* imagesLayout = new QVBoxLayout(imagesGroup);

    m_imageTable = new QTableWidget(this);
    m_imageTable->setColumnCount(6);
    m_imageTable->setHorizontalHeaderLabels({tr("Name"), tr("Hash"), tr("X"), tr("Y"), tr("Width"), tr("Height")});
    m_imageTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_imageTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_imageTable->horizontalHeader()->setStretchLastSection(true);
    m_imageTable->setMaximumHeight(150);
    connect(m_imageTable, &QTableWidget::itemSelectionChanged, this, &UIAtlasEditor::onImageSelectionChanged);
    imagesLayout->addWidget(m_imageTable);

    QHBoxLayout* imageButtons = new QHBoxLayout();
    m_addImageBtn = new QPushButton(tr("Add Image"), this);
    connect(m_addImageBtn, &QPushButton::clicked, this, &UIAtlasEditor::onAddImage);
    m_removeImageBtn = new QPushButton(tr("Remove"), this);
    m_removeImageBtn->setEnabled(false);
    connect(m_removeImageBtn, &QPushButton::clicked, this, &UIAtlasEditor::onRemoveImage);
    m_autoLayoutBtn = new QPushButton(tr("Auto Layout"), this);
    m_autoLayoutBtn->setToolTip(tr("Automatically arrange images in atlas"));
    connect(m_autoLayoutBtn, &QPushButton::clicked, this, &UIAtlasEditor::onAutoLayout);
    imageButtons->addWidget(m_addImageBtn);
    imageButtons->addWidget(m_removeImageBtn);
    imageButtons->addWidget(m_autoLayoutBtn);
    imageButtons->addStretch();
    imagesLayout->addLayout(imageButtons);

    layout->addWidget(imagesGroup);

    // Selected Image Properties Group
    QGroupBox* selectedGroup = new QGroupBox(tr("Selected Image Properties"), this);
    QGridLayout* selectedLayout = new QGridLayout(selectedGroup);
    row = 0;

    selectedLayout->addWidget(new QLabel(tr("Name:"), this), row, 0);
    m_imageNameEdit = new QLineEdit(this);
    m_imageNameEdit->setPlaceholderText(tr("image_name"));
    m_imageNameEdit->setEnabled(false);
    connect(m_imageNameEdit, &QLineEdit::textChanged, this, [this]() {
        int row = m_imageTable->currentRow();
        if (row >= 0 && row < m_images.size()) {
            m_images[row].name = m_imageNameEdit->text();
            updateImageTable();
            setModified(true);
        }
    });
    selectedLayout->addWidget(m_imageNameEdit, row, 1, 1, 2);
    row++;

    selectedLayout->addWidget(new QLabel(tr("Hash:"), this), row, 0);
    m_imageHashEdit = new QLineEdit(this);
    m_imageHashEdit->setPlaceholderText("0x00000000");
    m_imageHashEdit->setEnabled(false);
    connect(m_imageHashEdit, &QLineEdit::textChanged, this, [this]() {
        int row = m_imageTable->currentRow();
        if (row >= 0 && row < m_images.size()) {
            m_images[row].hash = m_imageHashEdit->text();
            updateImageTable();
            setModified(true);
        }
    });
    selectedLayout->addWidget(m_imageHashEdit, row, 1);
    m_calcHashBtn = new QPushButton(tr("Calculate"), this);
    m_calcHashBtn->setEnabled(false);
    connect(m_calcHashBtn, &QPushButton::clicked, this, &UIAtlasEditor::onCalculateHash);
    selectedLayout->addWidget(m_calcHashBtn, row, 2);
    row++;

    selectedLayout->addWidget(new QLabel(tr("Position:"), this), row, 0);
    QHBoxLayout* posLayout = new QHBoxLayout();
    posLayout->addWidget(new QLabel("X:", this));
    m_imagePosXSpin = new QSpinBox(this);
    m_imagePosXSpin->setRange(0, 8192);
    m_imagePosXSpin->setEnabled(false);
    connect(m_imagePosXSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
        int row = m_imageTable->currentRow();
        if (row >= 0 && row < m_images.size()) {
            m_images[row].posX = m_imagePosXSpin->value();
            updateImageTable();
            setModified(true);
        }
    });
    posLayout->addWidget(m_imagePosXSpin);
    posLayout->addWidget(new QLabel("Y:", this));
    m_imagePosYSpin = new QSpinBox(this);
    m_imagePosYSpin->setRange(0, 8192);
    m_imagePosYSpin->setEnabled(false);
    connect(m_imagePosYSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
        int row = m_imageTable->currentRow();
        if (row >= 0 && row < m_images.size()) {
            m_images[row].posY = m_imagePosYSpin->value();
            updateImageTable();
            setModified(true);
        }
    });
    posLayout->addWidget(m_imagePosYSpin);
    posLayout->addStretch();
    selectedLayout->addLayout(posLayout, row, 1, 1, 2);
    row++;

    selectedLayout->addWidget(new QLabel(tr("Size:"), this), row, 0);
    QHBoxLayout* imgSizeLayout = new QHBoxLayout();
    imgSizeLayout->addWidget(new QLabel("W:", this));
    m_imageWidthSpin = new QSpinBox(this);
    m_imageWidthSpin->setRange(1, 8192);
    m_imageWidthSpin->setValue(64);
    m_imageWidthSpin->setEnabled(false);
    connect(m_imageWidthSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
        int row = m_imageTable->currentRow();
        if (row >= 0 && row < m_images.size()) {
            m_images[row].width = m_imageWidthSpin->value();
            updateImageTable();
            setModified(true);
        }
    });
    imgSizeLayout->addWidget(m_imageWidthSpin);
    imgSizeLayout->addWidget(new QLabel("H:", this));
    m_imageHeightSpin = new QSpinBox(this);
    m_imageHeightSpin->setRange(1, 8192);
    m_imageHeightSpin->setValue(64);
    m_imageHeightSpin->setEnabled(false);
    connect(m_imageHeightSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
        int row = m_imageTable->currentRow();
        if (row >= 0 && row < m_images.size()) {
            m_images[row].height = m_imageHeightSpin->value();
            updateImageTable();
            setModified(true);
        }
    });
    imgSizeLayout->addWidget(m_imageHeightSpin);
    imgSizeLayout->addStretch();
    selectedLayout->addLayout(imgSizeLayout, row, 1, 1, 2);

    layout->addWidget(selectedGroup);

    // Preview Group
    QGroupBox* previewGroup = new QGroupBox(tr("Atlas Preview"), this);
    QVBoxLayout* previewLayout = new QVBoxLayout(previewGroup);

    m_previewLabel = new QLabel(this);
    m_previewLabel->setMinimumSize(256, 256);
    m_previewLabel->setMaximumSize(256, 256);
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setStyleSheet("background-color: #2d2d2d; border: 1px solid #3c3c3c;");
    m_previewLabel->setText(tr("Atlas preview not available"));
    previewLayout->addWidget(m_previewLabel, 0, Qt::AlignCenter);

    layout->addWidget(previewGroup);
    layout->addStretch();

    scrollArea->setWidget(scrollContent);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scrollArea);
}

void UIAtlasEditor::setAsset(const Asset& asset, Project* project)
{
    AssetEditorBase::setAsset(asset, project);

    m_pathEdit->setText(asset.path());
    m_guidEdit->setText(asset.guid());

    const QVariantMap& metadata = asset.metadata();

    m_atlasNameEdit->setText(metadata.value("atlasName", "").toString());
    m_texturePathEdit->setText(metadata.value("texturePath", "").toString());
    m_atlasWidthSpin->setValue(metadata.value("atlasWidth", 2048).toInt());
    m_atlasHeightSpin->setValue(metadata.value("atlasHeight", 2048).toInt());

    // Load images
    m_images.clear();
    QVariantList imageList = metadata.value("images").toList();
    for (const QVariant& img : imageList) {
        QVariantMap imgMap = img.toMap();
        AtlasImage image;
        image.name = imgMap.value("name").toString();
        image.hash = imgMap.value("hash").toString();
        image.posX = imgMap.value("posX", 0).toInt();
        image.posY = imgMap.value("posY", 0).toInt();
        image.width = imgMap.value("width", 64).toInt();
        image.height = imgMap.value("height", 64).toInt();
        m_images.append(image);
    }
    updateImageTable();

    setModified(false);
}

void UIAtlasEditor::clearAsset()
{
    AssetEditorBase::clearAsset();

    m_pathEdit->clear();
    m_guidEdit->clear();
    m_atlasNameEdit->clear();
    m_texturePathEdit->clear();
    m_atlasWidthSpin->setValue(2048);
    m_atlasHeightSpin->setValue(2048);
    m_images.clear();
    updateImageTable();
    m_previewLabel->setText(tr("Atlas preview not available"));
}

void UIAtlasEditor::onBrowseTexture()
{
    QString startDir;
    if (m_project) {
        startDir = m_project->absoluteAssetsDir();
    }

    QString path = QFileDialog::getOpenFileName(
        this,
        tr("Select Atlas Texture"),
        startDir,
        tr("Texture Files (*.dds *.png *.tga);;All Files (*)")
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
        m_texturePathEdit->setText(path);
        setModified(true);
    }
}

void UIAtlasEditor::onAddImage()
{
    AtlasImage image;
    image.name = QString("image_%1").arg(m_images.size());
    image.hash = "";
    image.posX = 0;
    image.posY = 0;
    image.width = 64;
    image.height = 64;
    m_images.append(image);
    updateImageTable();
    m_imageTable->selectRow(m_images.size() - 1);
    setModified(true);
}

void UIAtlasEditor::onRemoveImage()
{
    int row = m_imageTable->currentRow();
    if (row >= 0 && row < m_images.size()) {
        m_images.removeAt(row);
        updateImageTable();
        setModified(true);
    }
}

void UIAtlasEditor::onImageSelectionChanged()
{
    int row = m_imageTable->currentRow();
    bool hasSelection = (row >= 0 && row < m_images.size());

    m_removeImageBtn->setEnabled(hasSelection);
    m_imageNameEdit->setEnabled(hasSelection);
    m_imageHashEdit->setEnabled(hasSelection);
    m_calcHashBtn->setEnabled(hasSelection);
    m_imagePosXSpin->setEnabled(hasSelection);
    m_imagePosYSpin->setEnabled(hasSelection);
    m_imageWidthSpin->setEnabled(hasSelection);
    m_imageHeightSpin->setEnabled(hasSelection);

    if (hasSelection) {
        const AtlasImage& image = m_images[row];

        m_imageNameEdit->blockSignals(true);
        m_imageNameEdit->setText(image.name);
        m_imageNameEdit->blockSignals(false);

        m_imageHashEdit->blockSignals(true);
        m_imageHashEdit->setText(image.hash);
        m_imageHashEdit->blockSignals(false);

        m_imagePosXSpin->blockSignals(true);
        m_imagePosXSpin->setValue(image.posX);
        m_imagePosXSpin->blockSignals(false);

        m_imagePosYSpin->blockSignals(true);
        m_imagePosYSpin->setValue(image.posY);
        m_imagePosYSpin->blockSignals(false);

        m_imageWidthSpin->blockSignals(true);
        m_imageWidthSpin->setValue(image.width);
        m_imageWidthSpin->blockSignals(false);

        m_imageHeightSpin->blockSignals(true);
        m_imageHeightSpin->setValue(image.height);
        m_imageHeightSpin->blockSignals(false);
    } else {
        m_imageNameEdit->clear();
        m_imageHashEdit->clear();
        m_imagePosXSpin->setValue(0);
        m_imagePosYSpin->setValue(0);
        m_imageWidthSpin->setValue(64);
        m_imageHeightSpin->setValue(64);
    }
}

void UIAtlasEditor::onCalculateHash()
{
    int row = m_imageTable->currentRow();
    if (row < 0 || row >= m_images.size()) {
        return;
    }

    QString name = m_imageNameEdit->text();
    if (!name.isEmpty()) {
        // Use the uimg hash algorithm
        uint32_t hash = GuidCalculator::calculateUimgHash(name);
        QString hashStr = QString("0x%1").arg(hash, 8, 16, QChar('0')).toUpper();
        m_imageHashEdit->setText(hashStr);
        m_images[row].hash = hashStr;
        updateImageTable();
        setModified(true);
    }
}

void UIAtlasEditor::onAutoLayout()
{
    if (m_images.isEmpty()) {
        return;
    }

    // Simple row-based packing algorithm
    int atlasWidth = m_atlasWidthSpin->value();
    int padding = 2;
    int currentX = padding;
    int currentY = padding;
    int rowHeight = 0;

    for (int i = 0; i < m_images.size(); ++i) {
        AtlasImage& image = m_images[i];

        // Check if image fits in current row
        if (currentX + image.width + padding > atlasWidth) {
            // Move to next row
            currentX = padding;
            currentY += rowHeight + padding;
            rowHeight = 0;
        }

        image.posX = currentX;
        image.posY = currentY;
        currentX += image.width + padding;
        rowHeight = qMax(rowHeight, image.height);
    }

    updateImageTable();
    onImageSelectionChanged(); // Refresh selected image properties
    setModified(true);
}

void UIAtlasEditor::updateImageTable()
{
    int currentRow = m_imageTable->currentRow();
    m_imageTable->setRowCount(m_images.size());

    for (int i = 0; i < m_images.size(); ++i) {
        const AtlasImage& image = m_images[i];
        m_imageTable->setItem(i, 0, new QTableWidgetItem(image.name));
        m_imageTable->setItem(i, 1, new QTableWidgetItem(image.hash));
        m_imageTable->setItem(i, 2, new QTableWidgetItem(QString::number(image.posX)));
        m_imageTable->setItem(i, 3, new QTableWidgetItem(QString::number(image.posY)));
        m_imageTable->setItem(i, 4, new QTableWidgetItem(QString::number(image.width)));
        m_imageTable->setItem(i, 5, new QTableWidgetItem(QString::number(image.height)));
    }

    if (currentRow >= 0 && currentRow < m_images.size()) {
        m_imageTable->selectRow(currentRow);
    }
}
