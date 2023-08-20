#pragma once

#include "Process.h"
#include "Persistance/Reader_Process_Impl.h"
#include "Presenter/Presenter_Process_Impl.h"

template <my_db_credentials credential_ty>
class TProcess_Impl : public TProcess, public TProcess_Reader_Impl, public TProcess_Presenter_Impl {
   public:
      TProcess_Impl(): TProcess(), TProcess_Reader_Impl(), TProcess_Presenter_Impl() { }
};