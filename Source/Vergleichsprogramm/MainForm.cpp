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

    connect(ui.btnLogin, &QPushButton::clicked, this, [this]() { processes().Login(); });
    connect(ui.btnImportBln, &QPushButton::clicked, this, [this]() { processes().ImportBerlin(); });

    processes().Init(TMyForm(this, false));
    TProcess_Impl_Qt test; 
    std::cerr << test.ApplicationText() << "\n";
}

MainForm::~MainForm()
{}
