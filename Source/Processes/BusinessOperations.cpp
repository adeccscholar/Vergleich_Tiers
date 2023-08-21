#include "BusinessOperations.h"

#include <format>
using namespace std::string_literals;

TBusinessOperations::TBusinessOperations(void) {
   strApplication = "GeoDatenApp"s;
	iMajorVersion = 2;
	iMinorVersion = 0;
	}

TBusinessOperations::TBusinessOperations(TBusinessOperations const& ref) { 
	copy(ref); 
   }

TBusinessOperations::TBusinessOperations(TBusinessOperations&& ref) noexcept { 
	swap(ref); 
   }

TBusinessOperations::~TBusinessOperations(void) { 
   }

void TBusinessOperations::swap(TBusinessOperations& ref) noexcept { 
	std::swap(strApplication, ref.strApplication); 
	std::swap(iMajorVersion,  ref.iMajorVersion);
	std::swap(iMinorVersion,  ref.iMinorVersion);
   }

void TBusinessOperations::copy(TBusinessOperations const& ref) {
	strApplication = ref.strApplication;
	iMajorVersion  = ref.iMajorVersion;
	iMinorVersion  = ref.iMinorVersion;
   }

std::string TBusinessOperations::ApplicationText(void) {
	return std::format("{} {}.{}", strApplication, iMajorVersion, iMinorVersion);
   }
