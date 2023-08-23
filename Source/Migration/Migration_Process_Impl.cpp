﻿#include "Migration_Process_Impl.h"
#include <iostream>
#include <format>

TProcess_Migration_Impl::TProcess_Migration_Impl(void) : TProcess_Migration() {
   std::cerr << "constructor for TProcess_Migration_Impl called\n";
   }


void TProcess_Migration_Impl::ReadBerlinFromFile(std::string const& strFile) {
   std::clog << std::format("migrate data for Berlin from file \"{}\"\n", strFile);
   }
