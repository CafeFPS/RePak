#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTabWidget>

/**
 * Application preferences dialog
 */
class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget* parent = nullptr);
    ~PreferencesDialog() override;

private slots:
    void browseRepakPath();
    void browseGameDir();
    void browseProjectsDir();
    void apply();
    void reset();

private:
    void setupUi();
    void loadSettings();
    void saveSettings();

private:
    QTabWidget* m_tabWidget;

    // Paths
    QLineEdit* m_repakPathEdit;
    QLineEdit* m_gameDirEdit;
    QLineEdit* m_projectsDirEdit;

    // Build
    QSpinBox* m_compressLevelSpin;
    QSpinBox* m_compressWorkersSpin;
    QCheckBox* m_autoCompressCheck;
    QCheckBox* m_copyToGameCheck;

    // Editor
    QSpinBox* m_previewSizeSpin;
    QCheckBox* m_showPreviewsCheck;
    QCheckBox* m_enable3DCheck;

    // General
    QCheckBox* m_darkThemeCheck;
    QCheckBox* m_confirmDialogsCheck;
    QSpinBox* m_autoSaveSpin;
    QCheckBox* m_rememberLastCheck;
};

#endif // PREFERENCESDIALOG_H
