#ifndef BUILDPROGRESSWIDGET_H
#define BUILDPROGRESSWIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QElapsedTimer>

/**
 * Build progress display widget
 */
class BuildProgressWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BuildProgressWidget(QWidget* parent = nullptr);
    ~BuildProgressWidget() override;

    void setProgress(int current, int total);
    void setOperation(const QString& operation);
    void reset();

signals:
    void cancelRequested();

private:
    void setupUi();
    void updateTimeEstimate();

private:
    QProgressBar* m_progressBar;
    QLabel* m_operationLabel;
    QLabel* m_timeLabel;
    QPushButton* m_cancelButton;
    QElapsedTimer m_timer;
    int m_lastCurrent;
};

#endif // BUILDPROGRESSWIDGET_H
