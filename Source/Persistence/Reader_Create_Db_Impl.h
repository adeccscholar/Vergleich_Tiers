#pragma once

#include "Processes/Reader_Create_Db.h"
#include "Reader_Process_Impl.h"

class TProcess_Reader_Create_Impl : virtual public TProcess_Reader_Create, virtual public TProcess_Reader_Impl {
   public:
      TProcess_Reader_Create_Impl() : TProcess_Reader_Create(), TProcess_Reader_Impl() { Trace("constructor for TProcess_Reader_Create_Impl called."s); }
      TProcess_Reader_Create_Impl(TProcess_Reader_Create_Impl const&) = delete;
      TProcess_Reader_Create_Impl(TProcess_Reader_Create_Impl&&) noexcept = delete;
      virtual ~TProcess_Reader_Create_Impl() = default;

      TProcess_Reader_Create_Impl& operator = (TProcess_Reader_Create_Impl const&) = delete;
      TProcess_Reader_Create_Impl& operator = (TProcess_Reader_Create_Impl&&) noexcept = delete;
   };
