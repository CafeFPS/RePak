/**
 * RePak GUI - A graphical interface for the RePak asset packing tool
 *
 * Copyright (c) 2024
 */

#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <iostream>

#include "ui/mainwindow.h"
#include "core/settings.h"

int main(int argc, char *argv[])
{
    // Enable console output for debugging
    std::cout << "=== RePak GUI Starting ===" << std::endl;
    std::cout << "Qt Version: " << qVersion() << std::endl;

    QApplication app(argc, argv);

    qDebug() << "Application created";

    // Application metadata
    app.setApplicationName("RePak GUI");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("RePak");
    app.setOrganizationDomain("repak.dev");

    // Set Fusion style for consistent cross-platform look
    app.setStyle(QStyleFactory::create("Fusion"));

    // Apply dark theme
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));

    app.setPalette(darkPalette);

    // Load custom stylesheet
    QFile styleFile(":/styles/dark.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }

    // Load application settings
    qDebug() << "Loading settings...";
    Settings::instance().load();
    qDebug() << "Settings loaded";

    // Create and show main window
    qDebug() << "Creating main window...";
    MainWindow mainWindow;
    qDebug() << "Main window created, showing...";
    mainWindow.show();
    qDebug() << "Main window shown";

    // Handle command line arguments (open project file)
    QStringList args = app.arguments();
    if (args.size() > 1) {
        QString projectFile = args.at(1);
        if (QFile::exists(projectFile)) {
            mainWindow.openProject(projectFile);
        }
    }

    return app.exec();
}
