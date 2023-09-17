#include "Process_Impl_Qt.h"

#include "Persistence/Reader_Process_Impl.h"
#include <adecc_Scholar/MyForm.h>
#include <iostream>

namespace test_space {

// --------------------------------------------------------------
// method to test the instantiation of the process structure
// --------------------------------------------------------------
void Test4ProcessInstantiation(std::ostream& out, TMyForm&& mainform) {
   {
      out << "\nInstantiation of the concrete business process\n";
      TProcess_Impl_Qt test_proc;
      out << "Check Version: " << test_proc.ApplicationText() << "\n";

      out << "\nInstantiation of the sub process for persistence with test template\n";
      TProcess_Reader_Impl_Test test_subproc;
      out << "Check Testversion: " << test_subproc.ApplicationText() << "\n";
      auto [server, database]= test_subproc.GetConnectionInformations();
      out << "Server: " << server << " / Database: " << database << "\n";
   }
   out << "\nInstantiation of the sub process for presenterwith test template\n";
   TProcess_Presenter_Impl_Qt_Test test_sub_qt;
   test_sub_qt.SetForm(std::move(mainform));
   out << "Check Qt - Version: " << test_sub_qt.ApplicationText() << "\n";
   test_sub_qt.ShowInformationForm("test routine"s, "test for subprocess successful prepared"s, ""s);
   out << "\nTest passed.\n\n";
   }

}
