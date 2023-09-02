#pragma once

#include "Reader_Process.h"

class TProcess_Reader_System : virtual public TProcess_Reader {
   public:
      TProcess_Reader_System() : TProcess_Reader() { Trace("constructor for TProcess_Reader_System called."s); }
      TProcess_Reader_System(TProcess_Reader_System const&) = delete;
      TProcess_Reader_System(TProcess_Reader_System&&) noexcept = delete;
      virtual ~TProcess_Reader_System() = default;

      TProcess_Reader_System& operator = (TProcess_Reader_System const&) = delete;
      TProcess_Reader_System& operator = (TProcess_Reader_System&&) noexcept = delete;
   };
