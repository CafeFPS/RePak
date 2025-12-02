#ifndef SHADEREDITOR_H
#define SHADEREDITOR_H

#include "asseteditorbase.h"

/**
 * Editor for shader assets
 */
class ShaderEditor : public AssetEditorBase
{
    Q_OBJECT

public:
    explicit ShaderEditor(QWidget* parent = nullptr);
    ~ShaderEditor() override;

    AssetType supportedType() const override { return AssetType::Shader; }

private:
    void setupUi();
};

#endif // SHADEREDITOR_H
