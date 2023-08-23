#pragma once

#include "Processes/Process_Impl.h"
#include "Presenter_Process_Impl_Qt.h"


/*
class TProcess_Impl_Qt : public TProcess_Impl, public TProcess_Presenter_Impl_Qt {
public:
   TProcess_Impl_Qt() : TProcess_Impl(), TProcess_Presenter_Impl_Qt() { }
   };
*/

using TProcess_Impl_Qt = TProcess_Helper<TProcess_Impl, TProcess_Presenter_Impl_Qt>;
