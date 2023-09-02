#include "BusinessOperations.h"
#include <adecc_Tools/MyTrace.h>
#include <iostream>
#include <format>
using namespace std::string_literals;

TBusinessOperations::TBusinessOperations(void) {
   Trace("constructor for TBusinessOperations called."s);
   }

std::string TBusinessOperations::ApplicationText(void) const {
   return std::format("{} {}.{}", strApplication, iMajorVersion, iMinorVersion);
   }

