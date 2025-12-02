cmake_minimum_required(VERSION 3.20)
project(RePakGUI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Concurrent
)

# Source files
set(CORE_SOURCES
    src/core/project.cpp
    src/core/project.h
    src/core/asset.cpp
    src/core/asset.h
    src/core/buildmanager.cpp
    src/core/buildmanager.h
    src/core/settings.cpp
    src/core/settings.h
    src/core/repakprocess.cpp
    src/core/repakprocess.h
)

set(UI_SOURCES
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/projectsettingsdialog.cpp
    src/ui/projectsettingsdialog.h
    src/ui/preferencesdialog.cpp
    src/ui/preferencesdialog.h
    src/ui/aboutdialog.cpp
    src/ui/aboutdialog.h
)

set(WIDGET_SOURCES
    src/widgets/assetbrowser.cpp
    src/widgets/assetbrowser.h
    src/widgets/assetfilterwidget.cpp
    src/widgets/assetfilterwidget.h
    src/widgets/buildlogwidget.cpp
    src/widgets/buildlogwidget.h
    src/widgets/buildprogresswidget.cpp
    src/widgets/buildprogresswidget.h
    src/widgets/projectdashboard.cpp
    src/widgets/projectdashboard.h
    src/widgets/statusbar.cpp
    src/widgets/statusbar.h
)

set(EDITOR_SOURCES
    src/editors/asseteditorbase.cpp
    src/editors/asseteditorbase.h
    src/editors/textureeditor.cpp
    src/editors/textureeditor.h
    src/editors/materialeditor.cpp
    src/editors/materialeditor.h
    src/editors/modeleditor.cpp
    src/editors/modeleditor.h
    src/editors/shadereditor.cpp
    src/editors/shadereditor.h
    src/editors/datatableeditor.cpp
    src/editors/datatableeditor.h
    src/editors/uiatlaseditor.cpp
    src/editors/uiatlaseditor.h
)

set(MODEL_SOURCES
    src/models/assetlistmodel.cpp
    src/models/assetlistmodel.h
    src/models/assettreemodel.cpp
    src/models/assettreemodel.h
    src/models/assetfilterproxymodel.cpp
    src/models/assetfilterproxymodel.h
)

set(UTIL_SOURCES
    src/utils/guidcalculator.cpp
    src/utils/guidcalculator.h
    src/utils/jsonutils.cpp
    src/utils/jsonutils.h
    src/utils/fileutils.cpp
    src/utils/fileutils.h
    src/utils/validation.cpp
    src/utils/validation.h
)

set(TOOL_SOURCES
    src/tools/guidcalculatordialog.cpp
    src/tools/guidcalculatordialog.h
    src/tools/uimghashdialog.cpp
    src/tools/uimghashdialog.h
    src/tools/pakinspectordialog.cpp
    src/tools/pakinspectordialog.h
    src/tools/compressiondialog.cpp
    src/tools/compressiondialog.h
    src/tools/batchoperationsdialog.cpp
    src/tools/batchoperationsdialog.h
    src/tools/validationreportdialog.cpp
    src/tools/validationreportdialog.h
)

# Resources
set(RESOURCES
    resources/resources.qrc
)

# Main executable
add_executable(RePakGUI
    src/main.cpp
    ${CORE_SOURCES}
    ${UI_SOURCES}
    ${WIDGET_SOURCES}
    ${EDITOR_SOURCES}
    ${MODEL_SOURCES}
    ${UTIL_SOURCES}
    ${TOOL_SOURCES}
    ${RESOURCES}
)

target_include_directories(RePakGUI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/editors
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tools
)

target_link_libraries(RePakGUI PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Concurrent
)

# Windows-specific settings
if(WIN32)
    # Set to FALSE for debug console, TRUE for release (no console)
    option(REPAK_GUI_CONSOLE "Show console window for debugging" ON)

    if(REPAK_GUI_CONSOLE)
        set_target_properties(RePakGUI PROPERTIES
            WIN32_EXECUTABLE FALSE
        )
    else()
        set_target_properties(RePakGUI PROPERTIES
            WIN32_EXECUTABLE TRUE
        )
    endif()
endif()

# Install
install(TARGETS RePakGUI
    RUNTIME DESTINATION bin
)
