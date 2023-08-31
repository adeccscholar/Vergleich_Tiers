#pragma once

#include "Processes/Process_Impl.h"
#include "Presenter_Process_Impl_Qt.h"

using TProcess_Impl_Qt = concrete_process<TProcess_Helper<TProcess_Impl, TProcess_Presenter_Impl_Qt>>;



