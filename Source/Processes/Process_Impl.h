#pragma once

#include "Processes/Process.h"
#include "Persistance/Reader_Process_Impl.h"
#include "Presenter/Presenter_Process_Impl.h"

class TProcess_Impl : virtual public TProcess, virtual public TProcess_Reader_Impl, virtual public TProcess_Presenter_Impl {
public:
   TProcess_Impl() : TProcess(), TProcess_Reader_Impl(), TProcess_Presenter_Impl() { }
};
