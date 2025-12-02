#ifndef MATERIALEDITOR_H
#define MATERIALEDITOR_H

#include "asseteditorbase.h"

#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>

/**
 * Editor for material assets (matl)
 * Supports Respawn material properties including:
 * - Surface type
 * - Shader selection
 * - Texture slots
 * - Material flags
 */
class MaterialEditor : public AssetEditorBase
{
    Q_OBJECT

public:
    explicit MaterialEditor(QWidget* parent = nullptr);
    ~MaterialEditor() override;

    AssetType supportedType() const override { return AssetType::Material; }

    void setAsset(const Asset& asset, Project* project) override;
    void clearAsset() override;

private slots:
    void onAddTexture();
    void onRemoveTexture();
    void onBrowseTexture();
    void onTextureSelected(int row);

private:
    void setupUi();
    void updateTextureList();

    // Basic properties
    QLineEdit* m_pathEdit;
    QLineEdit* m_guidEdit;
    QComboBox* m_surfaceTypeCombo;
    QComboBox* m_shaderCombo;

    // Texture slots
    QListWidget* m_textureList;
    QPushButton* m_addTextureBtn;
    QPushButton* m_removeTextureBtn;
    QPushButton* m_browseTextureBtn;

    // Selected texture properties
    QLineEdit* m_texturePathEdit;
    QComboBox* m_textureSlotCombo;

    // Material flags
    QCheckBox* m_alphaBlendCheck;
    QCheckBox* m_noShadowCheck;
    QCheckBox* m_doubleSidedCheck;
    QCheckBox* m_emissiveCheck;

    // Material parameters
    QDoubleSpinBox* m_albedoTintR;
    QDoubleSpinBox* m_albedoTintG;
    QDoubleSpinBox* m_albedoTintB;
    QDoubleSpinBox* m_emissiveMultiplier;
    QDoubleSpinBox* m_glossiness;

    struct TextureSlot {
        QString path;
        QString slotType;
    };
    QList<TextureSlot> m_textures;
};

#endif // MATERIALEDITOR_H
