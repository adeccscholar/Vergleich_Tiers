#include "MainForm.h"

#include "System/Test/Test4System.h"
#include "Test4Process.h"

#include "Process_Impl_Qt.h"
#include <QAction>


MainForm::MainForm(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.statusBar->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
    statusLabel = new QLabel(this);
    statusLabel->setObjectName("sbMain");
    statusLabel->setText("sbMain");
    ui.statusBar->addPermanentWidget(statusLabel);

    connect(ui.btnLogin,           &QPushButton::clicked, this, [this]() { processes().Login(); });
    connect(ui.btnCreateBerlinOld, &QPushButton::clicked, this, [this]() { processes().CreateStructureBlnOld(); });
    connect(ui.btnImportBln,       &QPushButton::clicked, this, [this]() { processes().ImportBerlinOld(); });

    connect(ui.actLoginDatabase,   &QAction::triggered, [this]() { processes().Login(); });
    connect(ui.actExit,            &QAction::triggered, [this]() { processes().Close(); close();  });
    connect(ui.actCreateBlnOld,    &QAction::triggered, [this]() { processes().CreateStructureBlnOld(); });
    connect(ui.actImportBlnOld,    &QAction::triggered, [this]() { processes().ImportBerlinOld(); });


    processes().Init(TMyForm(this, false));

    Test();
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


// --------------------------------------------------------------
//  test routine 
// --------------------------------------------------------------
void MainForm::Test() {
   using namespace test_space;
   //Test4ProcessInstantiation(std::cerr, TMyForm(this, false));
   Test4Distance(std::cerr);
   Test4Angle(std::cerr);
   Test4Point(std::cerr);

   }
