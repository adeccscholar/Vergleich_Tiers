
#include "Process.h"
#include <adecc_Scholar/MyForm.h>
#include <adecc_Database/MyDatabaseCredentials.h>

TProcess::TProcess() : Process_Base() {
	Trace("constructor for TProcess called."s);
	}



std::string TProcess::ApplicationText(void) const {
	return std::format("{} ({})", TBusinessOperations::ApplicationText(), iBuild);
   }


std::string TProcess::CaptionForMainForm(void) const {
	if (!boProcessIsInitialized) {
		return std::format("application \"{}\" not initialized ...", ApplicationText());
	   }
	else [[likely]] {
		if(IsConnectedToDatabase()) [[likely]] {
			auto [strDatabase, strServerType] = GetConnectionInformations();
			return std::format("application \"{}\" connected to {} / {} ...", ApplicationText(), strDatabase, strServerType);
			}
		else
			return std::format("application \"{}\" not connected ...", ApplicationText());
	   }
}

void TProcess::Init(TMyForm&& form) {
	InitMainForm(std::forward<TMyForm>(form), CaptionForMainForm());
	boProcessIsInitialized = true;
	SetMainFormCaption(CaptionForMainForm());
	std::clog << std::format("application {} is successful started and ready\n", ApplicationText());
   }

void TProcess::Close() {
	LogoutFromDb();
   }

void TProcess::Login(void) {
	auto [strDatabase, strServerType] = GetConnectionInformations();
	std::string strTheme = std::format("connecting to database {} / {}", strDatabase, strServerType);
	std::clog << strTheme << '\n';
	TMyWait wait;
	try {
		if (auto ret = LoginForm(strTheme, GetServerHasIntegratedSecurity(), false, ""); ret.has_value()) {
			auto const& [usr, pwd, isec] = *ret;
			if (auto ret = LoginToDb({ usr, pwd, GetServerHasIntegratedSecurity() && isec }) ; ret.has_value()) {
				Trace(std::format("application \"{}\" connected to {} / {} ...", ApplicationText(), strDatabase, strServerType));
			   ShowInformationForm(strTheme, ret.value());
				std::clog << std::format("application \"{}\" connected successful to {} / {}.\n", ApplicationText(), strDatabase, strServerType);
			   }
		   else { 
				auto const& [errorcode, msg, details] = ret.error();
				Trace(std::format("application \"{}\" not connected ...", ApplicationText()));
				ShowErrorForm(strTheme, msg, details);
				std::clog << msg << '\n';
			   }
	      }
		else {
			auto const& [errorcode, message, details] = ret.error();
			std::clog << message << '\n';
			std::function<void()> call_msg_dlg;
			switch(errorcode) {
			   case EMyErrorType::Userbreak:
					call_msg_dlg = std::bind(&TProcess_Presenter::ShowInformationForm, this, std::cref(strTheme), std::cref(message), std::cref(details));
				   break;
			   case EMyErrorType::InputError:
					call_msg_dlg = std::bind(&TProcess_Presenter::ShowErrorForm, this, std::cref(strTheme), std::cref(message), std::cref(details));
					break;
			   default:
					call_msg_dlg = std::bind(&TProcess_Presenter::ShowErrorForm, this, std::cref(strTheme), std::cref(message), std::cref(details));
			   }
			Trace(std::format("application \"{}\" not connected to database {} at {}, {}", ApplicationText(), strDatabase, strServerType, message));
			call_msg_dlg();
		   }
		SetMainFormCaption(CaptionForMainForm());
		}
	catch(std::exception const& ex) {
		ShowErrorForm(strTheme, ex.what());
		SetMainFormCaption(CaptionForMainForm());
	   }
   }

void TProcess::ImportBerlin(void) {
	std::clog << "importing data for Berlin\n";
	auto [ret, strFile] = ChooseFile("d:\\test");
	if (ret) {
		ReadBerlinFromFile(strFile);
		std::clog << std::format("data for Berlin from file \"{}\" migrated successful.\n", strFile);
	   }
	else {
		std::clog << "import of data for Berlin aborted\n";
	   }
	}