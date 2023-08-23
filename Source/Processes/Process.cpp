
#include "Process.h"
#include "adecc_Scholar/MyForm.h"
#include "adecc_Database/MyDatabaseCredentials.h"

TProcess::TProcess() : Process_Base() {
	iBuild = 1001;
	std::cerr << "constructor for TProcess called\n";
   }



std::string TProcess::ApplicationText(void) {
	return std::format("{} ({})", TBusinessOperations::ApplicationText(), iBuild);
   }



void TProcess::Init(TMyForm&& form) {
	InitMainForm(std::forward<TMyForm>(form), std::format("application \"{}\" not connected ...", ApplicationText()));
   }

void TProcess::Login(void) {
	auto [strDatabase, strServerType] = GetConnectionInformations();
	std::string strTheme = std::format("connecting to database {} / {}", strDatabase, strServerType);
	TMyWait wait;
	try {
		if (auto ret = LoginForm(strTheme, GetServerHasIntegratedSecurity(), false, ""); ret.has_value()) {
			auto const& [usr, pwd, isec] = *ret;
			if (auto [ret, msg] = LoginToDb({ usr, pwd, GetServerHasIntegratedSecurity() && isec }) ; ret) {
			   ShowInformationForm(strTheme, msg);
			   SetMainFormCaption(std::format("application \"{}\" connected to {} / {} ...", ApplicationText(), strDatabase, strServerType));
		      }
		   else { 
			   ShowErrorForm(strTheme, msg);
			   SetMainFormCaption(std::format("application \"{}\" not connected ...", ApplicationText()));
	         }
	      }
		else {
			switch(ret.error().first) {
			   case EMyErrorType::Userbreak:
				   ShowInformationForm(strTheme, ret.error().second);
				   SetMainFormCaption(std::format("application \"{}\" not connected ...", ApplicationText()));
				   break;
			   case EMyErrorType::InputError:
				   ShowErrorForm(strTheme, ret.error().second);
				   SetMainFormCaption(std::format("application \"{}\" not connected ...", ApplicationText()));
				   break;
			   default:
				   ShowErrorForm(strTheme, ret.error().second);
				   SetMainFormCaption(std::format("application \"{}\" not connected ...", ApplicationText()));
			   }
			}
		}
	catch(std::exception const& ex) {
		ShowErrorForm(strTheme, ex.what());
		SetMainFormCaption(std::format("application \"{}\" not connected ...", ApplicationText()));
	   }
   }

void TProcess::ImportBerlin(void) {
	auto [ret, strFile] = ChooseFile("d:\\test");
	if (ret) ReadBerlinFromFile(strFile);
	// Fehlerbehandlung
   }