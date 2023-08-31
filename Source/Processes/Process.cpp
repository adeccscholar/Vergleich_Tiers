
#include "Process.h"
#include <adecc_Scholar/MyForm.h>
#include <adecc_Database/MyDatabaseCredentials.h>

TProcess::TProcess() : Process_Base() {
	Trace("constructor for TProcess called."s);
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
				auto text = std::format("application \"{}\" connected to {} / {} ...", ApplicationText(), strDatabase, strServerType);
				Trace(text);
			   ShowInformationForm(strTheme, msg);
			   SetMainFormCaption(text);
		      }
		   else { 
				auto text = std::format("application \"{}\" not connected ...", ApplicationText());
				Trace(text);
				ShowErrorForm(strTheme, msg);
			   SetMainFormCaption(text);
	         }
	      }
		else {
			std::string strDetail = ""s;
			std::function<void()> call_msg_dlg;
			switch(ret.error().first) {
			   case EMyErrorType::Userbreak:
					strDetail = "user break"s;
					call_msg_dlg = std::bind(&TProcess_Presenter::ShowInformationForm, this, std::cref(strTheme), std::cref(ret.error().second));
				   break;
			   case EMyErrorType::InputError:
					strDetail = "input error"s;
					call_msg_dlg = std::bind(&TProcess_Presenter::ShowErrorForm, this, std::cref(strTheme), std::cref(ret.error().second));
					break;
			   default:
					strDetail = "unexpected error"s;
					call_msg_dlg = std::bind(&TProcess_Presenter::ShowErrorForm, this, std::cref(strTheme), std::cref(ret.error().second));
			   }
			auto text = std::format("application \"{}\" not connected to database {} at {}, {}", ApplicationText(), strDatabase, strServerType, strDetail);
			Trace(text);
			call_msg_dlg();
			SetMainFormCaption(text);
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