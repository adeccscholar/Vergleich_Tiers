#pragma once

#include "Processes/Reader_Overview_Db.h"
#include "Reader_Process_Impl.h"

class TProcess_Reader_Overview_Impl : virtual public TProcess_Reader_Overview, virtual public TProcess_Reader_Impl {
   public:
      TProcess_Reader_Overview_Impl() : TProcess_Reader_Overview(), TProcess_Reader_Impl() { Trace("constructor for TProcess_Reader_Overview_Impl called."s); }
      TProcess_Reader_Overview_Impl(TProcess_Reader_Overview_Impl const&) = delete;
      TProcess_Reader_Overview_Impl(TProcess_Reader_Overview_Impl&&) noexcept = delete;
      virtual ~TProcess_Reader_Overview_Impl() = default;

      TProcess_Reader_Overview_Impl& operator = (TProcess_Reader_Overview_Impl const&) = delete;
      TProcess_Reader_Overview_Impl& operator = (TProcess_Reader_Overview_Impl&&) noexcept = delete;
   };
