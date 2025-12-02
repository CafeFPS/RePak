#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>
#include <QLabel>

/**
 * Custom status bar with additional widgets
 */
class StatusBar : public QStatusBar
{
    Q_OBJECT

public:
    explicit StatusBar(QWidget* parent = nullptr);
    ~StatusBar() override;

    void setAssetCount(int count);
    void setSelectionCount(int count);
    void setBuildStatus(const QString& status);

private:
    QLabel* m_assetCountLabel;
    QLabel* m_selectionLabel;
    QLabel* m_buildStatusLabel;
};

#endif // STATUSBAR_H
