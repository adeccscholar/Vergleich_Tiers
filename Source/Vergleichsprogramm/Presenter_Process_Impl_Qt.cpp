#include "Presenter_Process_Impl_Qt.h"

#include "UserDlg.h"

TMyForm TProcess_Presenter_Impl_Qt::CreateLoginForm(TMyForm& parent) {
   return TMyForm(new UserDlg(parent.Form()), true);
}