#pragma once

#include "Processes/Process.h"
#include "Persistance/Reader_Process_Impl.h"
#include "Presenter_Process_Impl_Qt.h"

template <my_db_credentials credential_ty>
class TProcess_Impl_Qt : public TProcess, public TProcess_Reader_Impl, public TProcess_Presenter_Impl_Qt {
public:
   TProcess_Impl_Qt() : TProcess(), TProcess_Reader_Impl(), TProcess_Presenter_Impl() { }
   };