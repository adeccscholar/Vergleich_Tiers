#include "Presenter_Process_Impl_Qt.h"

#include "UserDlg.h"

TProcess_Presenter_Impl_Qt::TProcess_Presenter_Impl_Qt(void) : TProcess_Presenter_Impl() { 
   std::cerr << "constructor for TProcess_Presenter_Impl_Qt called\n"; 
   }


TMyForm TProcess_Presenter_Impl_Qt::CreateLoginForm(TMyForm& parent) {
   return TMyForm(new UserDlg(parent.Form()), true);
}