#pragma once

#include "Reader_Process.h"

class TProcess_Reader_Overview : virtual public TProcess_Reader {
   public:
      TProcess_Reader_Overview() : TProcess_Reader() { Trace("constructor for TProcess_Reader_Overview called."s); }
      TProcess_Reader_Overview(TProcess_Reader_Overview const&) = delete;
      TProcess_Reader_Overview(TProcess_Reader_Overview&&) noexcept = delete;
      virtual ~TProcess_Reader_Overview() = default;

      TProcess_Reader_Overview& operator = (TProcess_Reader_Overview const&) = delete;
      TProcess_Reader_Overview& operator = (TProcess_Reader_Overview&&) noexcept = delete;

      virtual void ReadBerlin_old(void) = 0;
   };
