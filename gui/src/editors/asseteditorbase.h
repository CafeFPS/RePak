#ifndef ASSETEDITORBASE_H
#define ASSETEDITORBASE_H

#include <QWidget>

#include "core/asset.h"

class Project;

/**
 * Base class for all asset editors
 */
class AssetEditorBase : public QWidget
{
    Q_OBJECT

public:
    explicit AssetEditorBase(QWidget* parent = nullptr);
    ~AssetEditorBase() override;

    virtual void setAsset(const Asset& asset, Project* project);
    virtual void clearAsset();

    virtual AssetType supportedType() const = 0;
    virtual bool hasUnsavedChanges() const { return m_modified; }

signals:
    void assetModified();

protected:
    void setModified(bool modified);

protected:
    Asset m_asset;
    Project* m_project;
    bool m_modified;
};

#endif // ASSETEDITORBASE_H
