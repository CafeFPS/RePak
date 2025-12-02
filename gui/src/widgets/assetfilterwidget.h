#ifndef ASSETFILTERWIDGET_H
#define ASSETFILTERWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QMap>
#include <QSet>

#include "core/asset.h"

/**
 * Filter widget for the asset browser
 */
class AssetFilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AssetFilterWidget(QWidget* parent = nullptr);
    ~AssetFilterWidget() override;

    QString searchText() const;
    QSet<AssetType> enabledTypes() const;

    void setAssetCounts(const QMap<AssetType, int>& counts);

public slots:
    void clear();
    void selectAllTypes();
    void deselectAllTypes();

signals:
    void filterChanged(const QString& filter);
    void typeFilterChanged(const QSet<AssetType>& types);
    void searchTextChanged(const QString& text);

private slots:
    void onSearchTextChanged();
    void onTypeCheckChanged();

private:
    void setupUi();
    void updateTypeLabel(AssetType type, int count);

private:
    QLineEdit* m_searchEdit;
    QMap<AssetType, QCheckBox*> m_typeCheckboxes;
    QMap<AssetType, int> m_assetCounts;
};

#endif // ASSETFILTERWIDGET_H
