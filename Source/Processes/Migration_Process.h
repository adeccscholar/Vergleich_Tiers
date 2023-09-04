#pragma once


#include <adecc_Tools/MyTrace.h>
#include "BaseOfProcesses.h"
#include <iostream>
#include <string>

using namespace std::string_literals;

class TProcess_Migration : virtual public TOperationsBase {
public:
   TProcess_Migration() : TOperationsBase() { Trace("constructor for TProcess_Migration called"s); }
   TProcess_Migration(TProcess_Migration const&) = delete;
   TProcess_Migration(TProcess_Migration&& ref) noexcept = delete;
   virtual ~TProcess_Migration() = default;

   TProcess_Migration& operator = (TProcess_Migration const&) = delete;
   TProcess_Migration& operator = (TProcess_Migration&& ref) noexcept = delete;

   // Bereich mit den Teilprozessen
   virtual void ReadBerlinFromFile(std::string const&) = 0;

};
