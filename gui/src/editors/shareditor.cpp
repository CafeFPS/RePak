#include "shareditor.h"

#include <QVBoxLayout>
#include <QLabel>

ShaderEditor::ShaderEditor(QWidget* parent)
    : AssetEditorBase(parent)
{
    setupUi();
}

ShaderEditor::~ShaderEditor()
{
}

void ShaderEditor::setupUi()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* titleLabel = new QLabel(tr("Shader Editor"), this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    layout->addWidget(titleLabel);

    QLabel* placeholder = new QLabel(tr("Shader editor coming soon..."), this);
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet("color: #888;");
    layout->addWidget(placeholder, 1);
}
