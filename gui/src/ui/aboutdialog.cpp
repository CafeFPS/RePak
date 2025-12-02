#include "aboutdialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::setupUi()
{
    setWindowTitle(tr("About RePak GUI"));
    setFixedSize(400, 300);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(10);

    // Title
    QLabel* titleLabel = new QLabel(tr("RePak GUI"), this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    // Version
    QLabel* versionLabel = new QLabel(tr("Version %1").arg(QApplication::applicationVersion()), this);
    versionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(versionLabel);

    layout->addSpacing(20);

    // Description
    QLabel* descLabel = new QLabel(
        tr("A graphical interface for the RePak\n"
           "asset packing tool for Respawn games."),
        this
    );
    descLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(descLabel);

    layout->addSpacing(20);

    // Links
    QLabel* linkLabel = new QLabel(
        tr("<a href='https://github.com'>GitHub</a>"),
        this
    );
    linkLabel->setOpenExternalLinks(true);
    linkLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(linkLabel);

    layout->addStretch();

    // Copyright
    QLabel* copyrightLabel = new QLabel(tr("Copyright 2024"), this);
    copyrightLabel->setStyleSheet("color: #888;");
    copyrightLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(copyrightLabel);

    // Close button
    QPushButton* closeButton = new QPushButton(tr("Close"), this);
    closeButton->setFixedWidth(100);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeButton, 0, Qt::AlignCenter);
}
