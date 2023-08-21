#include "MainForm.h"


#include "System/MyGeoLocation.h"

MainForm::MainForm(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.statusBar->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
    statusLabel = new QLabel(this);
    statusLabel->setObjectName("sbMain");
    statusLabel->setText("sbMain");
    ui.statusBar->addPermanentWidget(statusLabel);

    connect(ui.btnLogin, &QPushButton::clicked, this, [this]() { proc.Login(); });

    proc.Init(TMyForm(this, false));
}

MainForm::~MainForm()
{}
