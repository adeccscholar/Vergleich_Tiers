#include "BusinessOperations.h"
#include <adecc_Tools/MyTrace.h>
#include <iostream>
#include <format>
using namespace std::string_literals;

TBusinessOperations::TBusinessOperations(void) : TOperationsBase() {
   Trace("constructor for TBusinessOperations called."s);
   }

TBusinessOperations::~TBusinessOperations(void) {
  // Trace("destructor for TBusinessOperations called."s);
   }

std::string TBusinessOperations::Application(void) const {
   return strApplication;
}

std::string TBusinessOperations::ApplicationVersion(void) const {
   return std::format("{}.{}", iMajorVersion, iMinorVersion);
}

std::string TBusinessOperations::ApplicationText(void) const {
   return std::format("{} {}.{} ({})", strApplication, iMajorVersion, iMinorVersion, BuildNumber());
   }

