#pragma once

#include "Processes/Process.h"
#include "Persistance/Reader_Process_Impl.h"
#include "Presenter/Presenter_Process_Impl.h"
#include "Migration/Migration_Process_Impl.h"

/*
class TProcess_Impl : virtual public TProcess, virtual public TProcess_Reader_Impl, 
                      virtual public TProcess_Presenter_Impl, virtual public TProcess_Migration_Impl {
public:
   TProcess_Impl() : TProcess(), TProcess_Reader_Impl(), TProcess_Presenter_Impl(), TProcess_Migration_Impl() { }
   TProcess_Impl(TProcess_Impl const& ref) : TProcess(ref), TProcess_Reader_Impl(ref), 
                                             TProcess_Presenter_Impl(ref), 
                                             TProcess_Migration_Impl(ref) { }

   virtual ~TProcess_Impl(void) { }
};
*/


using TProcess_Impl = TProcess_Helper<TProcess, TProcess_Reader_Impl, TProcess_Presenter_Impl, TProcess_Migration_Impl>;
