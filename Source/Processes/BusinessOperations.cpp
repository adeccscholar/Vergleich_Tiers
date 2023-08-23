#include "BusinessOperations.h"
#include <iostream>
#include <format>
using namespace std::string_literals;

TBusinessOperations::TBusinessOperations(void) {
   strApplication = "GeoDatenApp"s;
	iMajorVersion = 2;
	iMinorVersion = 0;
	std::cerr << "constructor for TBusinessOperations called\n";
	}

std::string TBusinessOperations::ApplicationText(void) {
	return std::format("{} {}.{}", strApplication, iMajorVersion, iMinorVersion);
   }
