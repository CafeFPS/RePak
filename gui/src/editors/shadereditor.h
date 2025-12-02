#ifndef SHADEREDITOR_H
#define SHADEREDITOR_H

#include "asseteditorbase.h"

#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QListWidget>
#include <QPushButton>

/**
 * Editor for shader assets (shdr/shds)
 * Supports Respawn shader properties including:
 * - Shader type (vertex, pixel, compute, geometry)
 * - Shader model version
 * - Entry point
 * - Defines/macros
 * - Include paths
 */
class ShaderEditor : public AssetEditorBase
{
    Q_OBJECT

public:
    explicit ShaderEditor(QWidget* parent = nullptr);
    ~ShaderEditor() override;

    AssetType supportedType() const override { return AssetType::Shader; }

    void setAsset(const Asset& asset, Project* project) override;
    void clearAsset() override;

private slots:
    void onAddDefine();
    void onRemoveDefine();
    void onDefineSelected(int row);
    void onBrowseSource();

private:
    void setupUi();
    void updateDefinesList();

    // Basic properties
    QLineEdit* m_pathEdit;
    QLineEdit* m_guidEdit;

    // Shader type and version
    QComboBox* m_shaderTypeCombo;
    QComboBox* m_shaderModelCombo;

    // Source file
    QLineEdit* m_sourcePathEdit;
    QPushButton* m_browseSourceBtn;
    QLineEdit* m_entryPointEdit;

    // Shader code preview (read-only)
    QTextEdit* m_codePreview;

    // Compilation options
    QCheckBox* m_debugInfoCheck;
    QCheckBox* m_optimizeCheck;
    QComboBox* m_optimizeLevelCombo;

    // Preprocessor defines
    QListWidget* m_definesList;
    QPushButton* m_addDefineBtn;
    QPushButton* m_removeDefineBtn;
    QLineEdit* m_defineNameEdit;
    QLineEdit* m_defineValueEdit;

    struct ShaderDefine {
        QString name;
        QString value;
    };
    QList<ShaderDefine> m_defines;
};

#endif // SHADEREDITOR_H
