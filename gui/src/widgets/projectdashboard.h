#ifndef PROJECTDASHBOARD_H
#define PROJECTDASHBOARD_H

#include <QWidget>
#include <QLabel>

class Project;

/**
 * Project dashboard showing overview and statistics
 */
class ProjectDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectDashboard(QWidget* parent = nullptr);
    ~ProjectDashboard() override;

    void setProject(Project* project);
    void refresh();

private:
    void setupUi();
    void updateStatistics();

private:
    Project* m_project;

    QLabel* m_titleLabel;
    QLabel* m_versionLabel;
    QLabel* m_outputLabel;

    QLabel* m_totalAssetsLabel;
    QLabel* m_textureCountLabel;
    QLabel* m_materialCountLabel;
    QLabel* m_modelCountLabel;
    QLabel* m_shaderCountLabel;
    QLabel* m_otherCountLabel;
    QLabel* m_totalSizeLabel;
};

#endif // PROJECTDASHBOARD_H
