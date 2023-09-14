#include "MainForm.h"


#include "System/MyGeoLocation.h"
#include "Process_Impl_Qt.h"
#include <QAction>

#include <System/MyDistance.h>
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


void MainForm::Test() {
   //TProcess_Impl_Qt test_proc;
   //std::cerr << test_proc.ApplicationText() << "\n";

   //TProcess_Reader_Impl_Test test_subproc;
   //std::cerr << test_subproc.ApplicationText() << "\n";

   using namespace my_distance_literals;

   MyDistance<double, MyDistanceKind::meter> entfernung;
   entfernung.Distance(1000.0);
   MyDistance<double, MyDistanceKind::kilometer> entfernung2 { entfernung };
   bool cc = (entfernung == entfernung2);

   entfernung += 500._meter;

   MyDistance<double, MyDistanceKind::yards> tey = entfernung ;

   // play with MyAngle
   MySexagesimalAngle<int> bln_tv_tower_lat("52° 31' 16\""s), 
                           bln_tv_tower_lon("13° 24' 40\"");  // ungefähre Position Berliner Fernsehturm

   MyWGS84<double>  bln_tv_tower(52.1_deg, 15.2_deg);
   MyAngle<double, MyAngleKind::degree> t1(52.1_deg), t2(15.2_deg);
   //MyGeoLocation<double, MyGeodeticRefType::WGS84> bln_tv_tower2(t1, t2);
   //MyGeoLocation<double, MyGeodeticRefType::WGS84> bln_tv_tower2(52.1_deg, 15.2_deg);
   MyGeoLocation<double, MyGeodeticRefType::WGS84> bln_tv_tower2(bln_tv_tower_lat, bln_tv_tower_lon);

   auto test = 0.25_rad;
   MyAngle<double, MyAngleKind::degree> tx = test;
   test.Check();
   test *= 2;
   test += 10.0_deg;
   auto cmp = test.convert_to<MyAngleKind::degree>();
   std::cerr << test.convert_to<MyAngleKind::degree>() << "\n";

   bool check = (test == cmp);
   auto calc = test + cmp;

   MyAngle<double, MyAngleKind::degree> tmp;
   std::string w = "13";
   tmp.Angle(w);

   auto test2 = 120.0_deg;
   std::cerr << std::setprecision(4);
   for(auto const& number : { 45._deg, 90._deg, 120._deg } ) {
      std::cerr << "sin(" << number << ") = " << number.sin() << "\n";
      std::cerr << "cos(" << number << ") = " << number.cos() << "\n";
      }
   }
