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


    connect(ui.actLoginDatabase,   &QAction::triggered, [this]() { processes().Login(); });
    connect(ui.actExit,            &QAction::triggered, [this]() { processes().Close(); close();  });
    connect(ui.actCreateBlnOld,    &QAction::triggered, [this]() { processes().CreateStructureBlnOld(); });
    connect(ui.actImportBlnOld,    &QAction::triggered, [this]() { processes().ImportBerlinOld(); });

    connect(ui.actTestProcesses,   &QAction::triggered, [this]() { test_space::Test4ProcessInstantiation(std::cerr, TMyForm(this, false)); });
    connect(ui.actTestPath,        &QAction::triggered, []() { test_space::Test4Path(std::cerr); });
    connect(ui.actTestPositions,   &QAction::triggered, []() { test_space::Test4Positions(std::cerr); });
    connect(ui.actTestSafeVector,  &QAction::triggered, []() { test_space::Test4SafeVector(std::cerr); });

    connect(ui.actTestPoints, &QAction::triggered, []() { test_space::Test4Point(std::cerr); });
    connect(ui.actTestAngle, &QAction::triggered, []() { test_space::Test4Angle(std::cerr); });
    connect(ui.actTestDistance, &QAction::triggered, []() { test_space::Test4Distance(std::cerr); });
 //   connect(ui.actTestNumbers, &QAction::triggered, []() { test_space::Test4Numbers(std::cerr); });
    /*
    Test4Numbers(std::cerr);
    //Test4FPE(std::cerr);
    //Test4Distance(std::cerr);
    //Test4Angle(std::cerr);
    //Test4Point(std::cerr);
    */

    connect(ui.btnLogin,           &QPushButton::clicked, ui.actLoginDatabase, &QAction::trigger);
    connect(ui.btnCreateBerlinOld, &QPushButton::clicked, ui.actCreateBlnOld,  &QAction::trigger);
    connect(ui.btnImportBln,       &QPushButton::clicked, ui.actImportBlnOld,  &QAction::trigger);
    connect(ui.btnClose,           &QPushButton::clicked, ui.actExit,          &QAction::trigger);

    processes().Init(TMyForm(this, false));
    statusLabel->setText(qVersion());

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


// --------------------------------------------------------------
//  test routine 
// --------------------------------------------------------------
void MainForm::Test() {
   using namespace test_space;
   try {
      Test4ProcessInstantiation(std::cerr, TMyForm(this, false));
      //   Test4SafeVector(std::cerr);
      //   Test4Path(std::cerr);
      //   Test4Positions(std::cerr);
      // Test4Numbers(std::cerr);
      //Test4FPE(std::cerr);
      //Test4Distance(std::cerr);
      //Test4Angle(std::cerr);
      //Test4Point(std::cerr);
      }
   catch(std::exception const& ex) {
      std::cerr << ex.what() << "\n";
      }
   }
