#pragma once

#include "Process.h"
#include "Persistance/Reader_Process_Impl.h"
#include "Presenter/Presenter_Process_Impl.h"

class  TProcess_Presenter_Impl_Qt : virtual public TProcess_Presenter_Impl {
public:
   TProcess_Presenter_Impl_Qt(void);
   TProcess_Presenter_Impl_Qt(TProcess_Presenter_Impl_Qt const&) = delete;
   TProcess_Presenter_Impl_Qt(TProcess_Presenter_Impl_Qt&&) = delete;
   virtual ~TProcess_Presenter_Impl_Qt() = default;

private:
   virtual TMyForm CreateLoginForm(TMyForm& parent);

   void Check() { delete new TProcess_Presenter_Impl_Qt;  }
};


