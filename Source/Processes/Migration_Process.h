#pragma once

#include <iostream>
#include <string>

class TProcess_Migration {
public:
   TProcess_Migration() { std::cerr << "constructor for TProcess_Migration called\n"; }
   TProcess_Migration(TProcess_Migration const&) = delete;
   TProcess_Migration(TProcess_Migration&& ref) noexcept = delete;
   virtual ~TProcess_Migration() = default;

   void swap(TProcess_Migration& ref) noexcept {
   }

   void copy(TProcess_Migration const& ref) {
   }

   // Bereich mit den Teilprozessen
   virtual void ReadBerlinFromFile(std::string const&) = 0;

};
