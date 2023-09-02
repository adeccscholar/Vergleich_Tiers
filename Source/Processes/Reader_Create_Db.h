#pragma once

#include "Reader_Process.h"

class TProcess_Reader_Create : virtual public TProcess_Reader {
   public:
      TProcess_Reader_Create() : TProcess_Reader() { Trace("constructor for TProcess_Reader_Create called."s); }
      TProcess_Reader_Create(TProcess_Reader_Create const&) = delete;
      TProcess_Reader_Create(TProcess_Reader_Create&&) noexcept = delete;
      virtual ~TProcess_Reader_Create() = default;

      TProcess_Reader_Create& operator = (TProcess_Reader_Create const&) = delete;
      TProcess_Reader_Create& operator = (TProcess_Reader_Create&&) noexcept = delete;
   };
