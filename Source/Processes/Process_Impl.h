#pragma once

#include "Processes/Process.h"
#include "Persistance/Reader_Create_Db_Impl.h"
#include "Persistance/Reader_System_Db_Impl.h"
#include "Persistance/Reader_Overview_Db_Impl.h"
#include "Presenter/Presenter_Process_Impl.h"
#include "Migration/Migration_Process_Impl.h"


//using TProcess_Impl = TProcess_Helper<TProcess, TProcess_Reader_Impl, TProcess_Presenter_Impl, TProcess_Migration_Impl>;
using TProcess_Impl = TProcess_Helper<TProcess, TProcess_Reader_Create_Impl, TProcess_Reader_System_Impl, TProcess_Reader_Overview_Impl, TProcess_Presenter_Impl, TProcess_Migration_Impl>;
