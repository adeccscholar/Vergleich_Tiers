
#include "Presenter_Process_Impl_Qt.h"
#include "UserDlg.h"

#include <adecc_Tools/MyTrace.h>


TProcess_Presenter_Impl_Qt::TProcess_Presenter_Impl_Qt(void) : TProcess_Presenter_Impl() { 
   Trace<ShouldTrace>("constructor for TProcess_Presenter_Impl_Qt called."s);
   }


TMyForm TProcess_Presenter_Impl_Qt::CreateLoginForm(TMyForm& parent) {
   return TMyForm(new UserDlg(parent.Form()), true);
}