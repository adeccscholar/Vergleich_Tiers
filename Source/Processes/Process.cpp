
#include "Process.h"

TProcess::TProcess(void) : TBusinessOperations(), TProcess_Presenter(), TProcess_Reader() { 
   }

TProcess::TProcess(TProcess const& ref) : TBusinessOperations(ref), TProcess_Presenter(ref), TProcess_Reader(ref) {
   }

TProcess::TProcess(TProcess&& ref) noexcept {
   swap(ref);
   }

void TProcess::swap(TProcess& ref) noexcept {
	TBusinessOperations::swap(static_cast<TBusinessOperations&>(ref));
	TProcess_Presenter::swap(static_cast<TProcess_Presenter&>(ref));
	TProcess_Reader::swap(static_cast<TProcess_Reader&>(ref));
  }

void TProcess::copy(TProcess const& ref) {
	TBusinessOperations::copy(static_cast<TBusinessOperations const&>(ref));
	TProcess_Presenter::copy(static_cast<TProcess_Presenter const&>(ref));
	TProcess_Reader::copy(static_cast<TProcess_Reader const&>(ref));
   }

void TProcess::Init(TMyForm&& form) {
	InitMainForm(std::forward<TMyForm>(form), std::format("application \"{}\" not connected ...", ApplicationText()));
   }

void TProcess::Login(void) {
	auto [strDatabase, strServerType] = GetConnectionInformations();
	std::string strTheme = std::format("connecting to database {} / {}", strDatabase, strServerType);
	//TMyWait wait;
	try {
		auto [usr, pwd, isec] = LoginForm(strTheme, GetServerHasIntegratedSecurity(), false, "");
		auto [ret, msg] = LoginToDb( { usr, pwd, GetServerHasIntegratedSecurity() && isec } );
		if (ret) {
			ShowInformationForm(strTheme, msg);
			SetMainFormCaption(std::format("application \"{}\" connected to {} / {} ...", ApplicationText(), strDatabase, strServerType));
		   }
		else { 
			ShowErrorForm(strTheme, msg);
			SetMainFormCaption(std::format("application \"{}\" not connected ...", ApplicationText()));
	      }
	   }
	catch (TMy_InputError const& ex) {
		ShowErrorForm(strTheme, ex.what());
		SetMainFormCaption(std::format("application \"{}\" not connected ...", ApplicationText()));
	   }
	catch (TMy_UserBreak const& ex) {
		ShowInformationForm(strTheme, ex.what());
		SetMainFormCaption(std::format("application \"{}\" not connected ...", ApplicationText()));
	   }
   }