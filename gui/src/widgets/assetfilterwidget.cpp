#include "assetfilterwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QScrollArea>

AssetFilterWidget::AssetFilterWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

AssetFilterWidget::~AssetFilterWidget()
{
}

void AssetFilterWidget::setupUi()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    // Search box
    QLabel* searchLabel = new QLabel(tr("Search:"), this);
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText(tr("Filter assets..."));
    m_searchEdit->setClearButtonEnabled(true);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &AssetFilterWidget::onSearchTextChanged);

    layout->addWidget(searchLabel);
    layout->addWidget(m_searchEdit);

    // Type filters
    QGroupBox* typeGroup = new QGroupBox(tr("Asset Types"), this);
    QVBoxLayout* typeLayout = new QVBoxLayout(typeGroup);
    typeLayout->setSpacing(4);

    // Select/Deselect all buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* selectAllBtn = new QPushButton(tr("All"), this);
    selectAllBtn->setMaximumWidth(60);
    connect(selectAllBtn, &QPushButton::clicked, this, &AssetFilterWidget::selectAllTypes);

    QPushButton* deselectAllBtn = new QPushButton(tr("None"), this);
    deselectAllBtn->setMaximumWidth(60);
    connect(deselectAllBtn, &QPushButton::clicked, this, &AssetFilterWidget::deselectAllTypes);

    buttonLayout->addWidget(selectAllBtn);
    buttonLayout->addWidget(deselectAllBtn);
    buttonLayout->addStretch();
    typeLayout->addLayout(buttonLayout);

    // Create checkboxes for each asset type
    QList<AssetType> types = {
        AssetType::Texture,
        AssetType::Material,
        AssetType::Model,
        AssetType::Shader,
        AssetType::ShaderSet,
        AssetType::AnimRig,
        AssetType::AnimSequence,
        AssetType::Datatable,
        AssetType::UIImageAtlas,
        AssetType::Settings
    };

    for (AssetType type : types) {
        QString name = Asset::typeToDisplayName(type);
        QCheckBox* checkbox = new QCheckBox(name, this);
        checkbox->setChecked(true);
        checkbox->setProperty("assetType", static_cast<int>(type));
        connect(checkbox, &QCheckBox::stateChanged, this, &AssetFilterWidget::onTypeCheckChanged);
        m_typeCheckboxes[type] = checkbox;
        typeLayout->addWidget(checkbox);
    }

    layout->addWidget(typeGroup);
    layout->addStretch();
}

QString AssetFilterWidget::searchText() const
{
    return m_searchEdit->text();
}

QSet<AssetType> AssetFilterWidget::enabledTypes() const
{
    QSet<AssetType> types;
    for (auto it = m_typeCheckboxes.begin(); it != m_typeCheckboxes.end(); ++it) {
        if (it.value()->isChecked()) {
            types.insert(it.key());
        }
    }
    return types;
}

void AssetFilterWidget::setAssetCounts(const QMap<AssetType, int>& counts)
{
    m_assetCounts = counts;
    for (auto it = counts.begin(); it != counts.end(); ++it) {
        updateTypeLabel(it.key(), it.value());
    }
}

void AssetFilterWidget::clear()
{
    m_searchEdit->clear();
    selectAllTypes();
}

void AssetFilterWidget::selectAllTypes()
{
    for (QCheckBox* checkbox : m_typeCheckboxes) {
        checkbox->blockSignals(true);
        checkbox->setChecked(true);
        checkbox->blockSignals(false);
    }
    emit typeFilterChanged(enabledTypes());
}

void AssetFilterWidget::deselectAllTypes()
{
    for (QCheckBox* checkbox : m_typeCheckboxes) {
        checkbox->blockSignals(true);
        checkbox->setChecked(false);
        checkbox->blockSignals(false);
    }
    emit typeFilterChanged(enabledTypes());
}

void AssetFilterWidget::onSearchTextChanged()
{
    QString text = m_searchEdit->text();
    emit searchTextChanged(text);
    emit filterChanged(text);
}

void AssetFilterWidget::onTypeCheckChanged()
{
    emit typeFilterChanged(enabledTypes());
}

void AssetFilterWidget::updateTypeLabel(AssetType type, int count)
{
    if (m_typeCheckboxes.contains(type)) {
        QCheckBox* checkbox = m_typeCheckboxes[type];
        QString name = Asset::typeToDisplayName(type);
        checkbox->setText(QString("%1 (%2)").arg(name).arg(count));
    }
}
