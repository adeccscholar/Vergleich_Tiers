#pragma once

#include "Processes/Process.h"
#include "Persistance/Reader_Process_Impl.h"
#include "Presenter/Presenter_Process_Impl.h"
#include "Migration/Migration_Process_Impl.h"

using TProcess_Impl = TProcess_Helper<TProcess, TProcess_Reader_Impl, TProcess_Presenter_Impl, TProcess_Migration_Impl>;
