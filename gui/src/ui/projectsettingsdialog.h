#ifndef PROJECTSETTINGSDIALOG_H
#define PROJECTSETTINGSDIALOG_H

#include <QDialog>

class Project;

/**
 * Project settings dialog
 */
class ProjectSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectSettingsDialog(Project* project, QWidget* parent = nullptr);
    ~ProjectSettingsDialog() override;

private slots:
    void apply();

private:
    void setupUi();
    void loadFromProject();
    void saveToProject();

private:
    Project* m_project;
    // TODO: Add widgets for project settings
};

#endif // PROJECTSETTINGSDIALOG_H
