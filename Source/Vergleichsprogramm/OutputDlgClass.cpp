#include "OutputDlgClass.h"

OutputDlgClass::OutputDlgClass(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.btnClose, &QPushButton::clicked, this, [this]() { this->close(); });
}

OutputDlgClass::~OutputDlgClass()
{}
