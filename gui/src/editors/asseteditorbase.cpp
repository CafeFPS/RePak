#include "asseteditorbase.h"

AssetEditorBase::AssetEditorBase(QWidget* parent)
    : QWidget(parent)
    , m_project(nullptr)
    , m_modified(false)
{
}

AssetEditorBase::~AssetEditorBase()
{
}

void AssetEditorBase::setAsset(const Asset& asset, Project* project)
{
    m_asset = asset;
    m_project = project;
    m_modified = false;
}

void AssetEditorBase::clearAsset()
{
    m_asset = Asset();
    m_project = nullptr;
    m_modified = false;
}

void AssetEditorBase::setModified(bool modified)
{
    if (m_modified != modified) {
        m_modified = modified;
        if (modified) {
            emit assetModified();
        }
    }
}
