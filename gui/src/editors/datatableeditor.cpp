#include "datatableeditor.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>

#include "core/project.h"

DatatableEditor::DatatableEditor(QWidget* parent)
    : AssetEditorBase(parent)
{
    setupUi();
}

DatatableEditor::~DatatableEditor()
{
}

void DatatableEditor::setupUi()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* titleLabel = new QLabel(tr("Datatable Editor"), this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    layout->addWidget(titleLabel);

    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setAlternatingRowColors(true);
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(m_tableWidget, 1);
}

void DatatableEditor::setAsset(const Asset& asset, Project* project)
{
    AssetEditorBase::setAsset(asset, project);

    if (project && asset.path().endsWith(".csv", Qt::CaseInsensitive)) {
        QString fullPath = asset.absolutePath(project->absoluteAssetsDir());
        loadCsv(fullPath);
    }
}

void DatatableEditor::loadCsv(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);
    QStringList lines;
    while (!stream.atEnd()) {
        lines.append(stream.readLine());
    }
    file.close();

    if (lines.isEmpty()) {
        return;
    }

    // Parse CSV
    QStringList headers = lines[0].split(',');
    m_tableWidget->setColumnCount(headers.size());
    m_tableWidget->setHorizontalHeaderLabels(headers);
    m_tableWidget->setRowCount(lines.size() - 1);

    for (int row = 1; row < lines.size(); ++row) {
        QStringList cells = lines[row].split(',');
        for (int col = 0; col < cells.size() && col < headers.size(); ++col) {
            m_tableWidget->setItem(row - 1, col, new QTableWidgetItem(cells[col]));
        }
    }
}
