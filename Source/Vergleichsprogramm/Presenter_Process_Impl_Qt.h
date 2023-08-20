#pragma once

#include "Process.h"
#include "Persistance/Reader_Process_Impl.h"
#include "Presenter/Presenter_Process_Impl.h"

class  TProcess_Presenter_Impl_Qt : virtual public TProcess_Presenter_Impl {
private:
   TMyForm frm;  /// Hauptfester der Anwendung, wird in InitMainForm übernommen
   std::locale myLoc;

   static inline TStreamWrapper<Narrow> old_cout{ std::cout };
   static inline TStreamWrapper<Narrow> old_cerr{ std::cerr };
   static inline TStreamWrapper<Narrow> old_clog{ std::clog };

public:
   TProcess_Presenter_Impl_Qt(void) : TProcess_Presenter_Impl() { }
   TProcess_Presenter_Impl_Qt(TProcess_Presenter_Impl_Qt const& ref) : TProcess_Presenter_Impl(ref) { }
   virtual ~TProcess_Presenter_Impl_Qt() { }

private:
   virtual TMyForm CreateLoginForm(TMyForm& parent);
};


