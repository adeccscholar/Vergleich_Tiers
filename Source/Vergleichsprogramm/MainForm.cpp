#include "MainForm.h"


#include "System/MyGeoLocation.h"
#include "Process_Impl_Qt.h"

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

    //Test();
   }

MainForm::~MainForm()
{}

/// global variable with the business operations for this application
TProcess_Impl_Qt proc;

TBusinessOperations& MainForm::processes() { 
   return proc; 
   }

TBusinessOperations const& MainForm::processes() const { 
   return proc; 
   }

void MainForm::Test() {
   TProcess_Impl_Qt test;
   std::cerr << test.ApplicationText() << "\n";
   }
