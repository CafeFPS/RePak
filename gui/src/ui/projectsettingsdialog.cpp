#include "projectsettingsdialog.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

#include "core/project.h"

ProjectSettingsDialog::ProjectSettingsDialog(Project* project, QWidget* parent)
    : QDialog(parent)
    , m_project(project)
{
    setupUi();
    loadFromProject();
}

ProjectSettingsDialog::~ProjectSettingsDialog()
{
}

void ProjectSettingsDialog::setupUi()
{
    setWindowTitle(tr("Project Settings"));
    setMinimumSize(500, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    // TODO: Add project settings widgets

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply,
        this
    );

    connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, [this]() {
        apply();
        accept();
    });
    connect(buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &QDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ProjectSettingsDialog::apply);

    layout->addWidget(buttonBox);
}

void ProjectSettingsDialog::loadFromProject()
{
    // TODO: Load settings from project
}

void ProjectSettingsDialog::saveToProject()
{
    // TODO: Save settings to project
}

void ProjectSettingsDialog::apply()
{
    saveToProject();
}
