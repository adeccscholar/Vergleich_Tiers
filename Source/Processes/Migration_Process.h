#pragma once

#include <string>

class TProcess_Migration {
public:
   TProcess_Migration() = default;
   TProcess_Migration(TProcess_Migration const& ref) { copy(ref); }
   TProcess_Migration(TProcess_Migration&& ref) noexcept { swap(ref); }

   virtual ~TProcess_Migration() = default;

   void swap(TProcess_Migration& ref) noexcept {
   }

   void copy(TProcess_Migration const& ref) {
   }

   // Bereich mit den Teilprozessen
   virtual void ReadBerlinFromFile(std::string const&) = 0;

};
