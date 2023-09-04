#pragma once

#include "Process.h"
#include "Persistence/Reader_Process_Impl.h"
#include "Presenter/Presenter_Process_Impl.h"

class  TProcess_Presenter_Impl_Qt : virtual public TProcess_Presenter_Impl {
public:
   TProcess_Presenter_Impl_Qt(void);
   virtual ~TProcess_Presenter_Impl_Qt() = default;

   TProcess_Presenter_Impl_Qt(TProcess_Presenter_Impl_Qt const&) = delete;
   TProcess_Presenter_Impl_Qt(TProcess_Presenter_Impl_Qt&&) = delete;
   TProcess_Presenter_Impl_Qt& operator = (TProcess_Presenter_Impl_Qt const&) = delete;
   TProcess_Presenter_Impl_Qt& operator = (TProcess_Presenter_Impl_Qt&&) = delete;

private:
   virtual TMyForm CreateLoginForm(TMyForm& parent);

};


