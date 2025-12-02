#ifndef MODELEDITOR_H
#define MODELEDITOR_H

#include "asseteditorbase.h"

#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QTreeWidget>

/**
 * Editor for model assets (mdl_)
 * Supports Respawn model properties including:
 * - Model path and GUID
 * - Animation rig reference
 * - Physics properties
 * - LOD settings
 * - Material overrides
 */
class ModelEditor : public AssetEditorBase
{
    Q_OBJECT

public:
    explicit ModelEditor(QWidget* parent = nullptr);
    ~ModelEditor() override;

    AssetType supportedType() const override { return AssetType::Model; }

    void setAsset(const Asset& asset, Project* project) override;
    void clearAsset() override;

private slots:
    void onBrowseAnimRig();
    void onAddMaterial();
    void onRemoveMaterial();
    void onBrowseMaterial();
    void onMaterialSelected(int row);

private:
    void setupUi();
    void updateMaterialList();

    // Basic properties
    QLineEdit* m_pathEdit;
    QLineEdit* m_guidEdit;

    // Animation rig
    QLineEdit* m_animRigEdit;
    QPushButton* m_browseAnimRigBtn;

    // Model info (read-only, extracted from file)
    QLabel* m_vertexCountLabel;
    QLabel* m_triangleCountLabel;
    QLabel* m_lodCountLabel;
    QLabel* m_submeshCountLabel;

    // Physics settings
    QComboBox* m_physicsTypeCombo;
    QCheckBox* m_generatePhysicsCheck;

    // LOD settings
    QSpinBox* m_lodCountSpin;
    QTreeWidget* m_lodTree;

    // Material overrides
    QListWidget* m_materialList;
    QPushButton* m_addMaterialBtn;
    QPushButton* m_removeMaterialBtn;
    QPushButton* m_browseMaterialBtn;
    QLineEdit* m_materialPathEdit;

    // Streaming
    QCheckBox* m_streamMandatoryCheck;
    QCheckBox* m_streamOptionalCheck;

    struct MaterialOverride {
        QString originalName;
        QString replacementPath;
    };
    QList<MaterialOverride> m_materials;
};

#endif // MODELEDITOR_H
