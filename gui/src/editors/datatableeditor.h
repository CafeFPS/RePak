#ifndef DATATABLEEDITOR_H
#define DATATABLEEDITOR_H

#include "asseteditorbase.h"

#include <QTableWidget>

/**
 * Editor for datatable assets
 */
class DatatableEditor : public AssetEditorBase
{
    Q_OBJECT

public:
    explicit DatatableEditor(QWidget* parent = nullptr);
    ~DatatableEditor() override;

    AssetType supportedType() const override { return AssetType::Datatable; }
    void setAsset(const Asset& asset, Project* project) override;

private:
    void setupUi();
    void loadCsv(const QString& path);

private:
    QTableWidget* m_tableWidget;
};

#endif // DATATABLEEDITOR_H
