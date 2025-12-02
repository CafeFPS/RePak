#ifndef TEXTUREEDITOR_H
#define TEXTUREEDITOR_H

#include "asseteditorbase.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>

/**
 * Editor for texture assets
 */
class TextureEditor : public AssetEditorBase
{
    Q_OBJECT

public:
    explicit TextureEditor(QWidget* parent = nullptr);
    ~TextureEditor() override;

    void setAsset(const Asset& asset, Project* project) override;
    void clearAsset() override;
    AssetType supportedType() const override { return AssetType::Texture; }

private:
    void setupUi();
    void updatePreview();

private:
    QLabel* m_previewLabel;
    QLineEdit* m_guidEdit;
    QLineEdit* m_pathEdit;
    QComboBox* m_formatCombo;
    QLineEdit* m_widthEdit;
    QLineEdit* m_heightEdit;
};

#endif // TEXTUREEDITOR_H
