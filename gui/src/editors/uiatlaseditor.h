#ifndef UIATLASEDITOR_H
#define UIATLASEDITOR_H

#include "asseteditorbase.h"

#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>

/**
 * Editor for UI image atlas assets (uimg)
 * Supports Respawn UI atlas properties including:
 * - Atlas name and GUID
 * - Texture reference
 * - Image list with positions and sizes
 * - Hash generation for image names
 */
class UIAtlasEditor : public AssetEditorBase
{
    Q_OBJECT

public:
    explicit UIAtlasEditor(QWidget* parent = nullptr);
    ~UIAtlasEditor() override;

    AssetType supportedType() const override { return AssetType::UIImageAtlas; }

    void setAsset(const Asset& asset, Project* project) override;
    void clearAsset() override;

private slots:
    void onBrowseTexture();
    void onAddImage();
    void onRemoveImage();
    void onImageSelectionChanged();
    void onCalculateHash();
    void onAutoLayout();

private:
    void setupUi();
    void updateImageTable();

    // Basic properties
    QLineEdit* m_pathEdit;
    QLineEdit* m_guidEdit;
    QLineEdit* m_atlasNameEdit;

    // Texture reference
    QLineEdit* m_texturePathEdit;
    QPushButton* m_browseTextureBtn;

    // Atlas dimensions
    QSpinBox* m_atlasWidthSpin;
    QSpinBox* m_atlasHeightSpin;

    // Image list
    QTableWidget* m_imageTable;
    QPushButton* m_addImageBtn;
    QPushButton* m_removeImageBtn;
    QPushButton* m_autoLayoutBtn;

    // Selected image properties
    QLineEdit* m_imageNameEdit;
    QLineEdit* m_imageHashEdit;
    QPushButton* m_calcHashBtn;
    QSpinBox* m_imagePosXSpin;
    QSpinBox* m_imagePosYSpin;
    QSpinBox* m_imageWidthSpin;
    QSpinBox* m_imageHeightSpin;

    // Image preview
    QLabel* m_previewLabel;

    struct AtlasImage {
        QString name;
        QString hash;
        int posX;
        int posY;
        int width;
        int height;
    };
    QList<AtlasImage> m_images;
};

#endif // UIATLASEDITOR_H
