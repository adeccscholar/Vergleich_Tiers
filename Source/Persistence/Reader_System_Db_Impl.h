#pragma once

#include "Processes/Reader_System_Db.h"
#include "Reader_Process_Impl.h"

class TProcess_Reader_System_Impl : virtual public TProcess_Reader_System, virtual public TProcess_Reader_Impl {
   public:
      TProcess_Reader_System_Impl() : TProcess_Reader_System(), TProcess_Reader_Impl() { Trace("constructor for TProcess_Reader_System_Impl called."s); }
      TProcess_Reader_System_Impl(TProcess_Reader_System_Impl const&) = delete;
      TProcess_Reader_System_Impl(TProcess_Reader_System_Impl&&) noexcept = delete;
      virtual ~TProcess_Reader_System_Impl() = default;

      TProcess_Reader_System_Impl& operator = (TProcess_Reader_System_Impl const&) = delete;
      TProcess_Reader_System_Impl& operator = (TProcess_Reader_System_Impl&&) noexcept = delete;
   };
