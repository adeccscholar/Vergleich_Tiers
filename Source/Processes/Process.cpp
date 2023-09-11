
#include "Process.h"
#include <adecc_Scholar/MyForm.h>
#include <adecc_Database/MyDatabaseCredentials.h>

TProcess::TProcess() : Process_Base() {
	Trace("constructor for TProcess called."s);
	}


int TProcess::BuildNumber(void) const {
	return iBuild;
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
				auto const& [errorcode, msg, details] = AddTrace(ret.error(), "Login to database failed.");
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
			call_msg_dlg();
		   }
		SetMainFormCaption(CaptionForMainForm());
		}
	catch(std::exception const& ex) {
		ShowErrorForm(strTheme, ex.what());
		SetMainFormCaption(CaptionForMainForm());
	   }
   }

/**
 * @brief create the structure for the old data model for Berlin into selected database
 * @details In this method, the tables and views are created in order to be able to store the Berlin data 
 *          in the database in the old database model.
 * @param  
 * @return
*/
void TProcess::CreateStructureBlnOld(void) {
	const std::string strTheme{"create structure for Berlin"s };
	auto [strDatabase, strServerType] = GetConnectionInformations();
	std::clog << std::format("{} in Database {} / {}\n", strTheme, strDatabase, strServerType);
   if(auto ret = HasDbStructureForBerlin(); ret.has_value()) {
		if(ret.value().first == true) {
			auto ret_question = ShowQuestionForm(strTheme, "do you want drop existing views and tables"s, 
				                                  std::format("database: {} / {}\n\n{}", strDatabase, strServerType, ret.value().second));
			if (ret_question.has_value()) {
				if (ret_question.value() == true) { 
					if(auto ret_drop = DropDbStructureForBerlin(); !ret_drop.has_value()) {
						auto const& [error, msg, details] = ret_question.error();
						ShowErrorForm(strTheme, "Failed to delete the existing tables and views.",
							           std::format("{}\n{}\n\nactual position\n{}\nApplication: {}",
							                       msg, details, MyPosition(), ApplicationText()));
						return;
					   }
				   }
			   }
			else {
				auto const& [error, msg, details] = ret_question.error();
				ShowErrorForm(strTheme, "The building of the Berlin data structure was canceled by the user",
					           std::format("{}\n{}\n\nactual position\n{}\nApplication: {}",
					                       msg, details, MyPosition(), ApplicationText()));
				return;
			   }
		   }
		if(auto ret_execute = CreateDbStructureForBerlin(); ret_execute.has_value()) {
			ShowInformationForm(strTheme, "The database structures have been successfully created.", "");
		   }
		else {
			auto const& [error, msg, details] = ret_execute.error();
			ShowErrorForm(strTheme, "Error encountered while executing the statements for creating the database structure.",
				std::format("{}\n{}\n\nactual position\n{}\nApplication: {}",
				            msg, details, MyPosition(), ApplicationText()));
		   }
	   }
	else {
		auto const& [error, msg, details] = ret.error();
		ShowErrorForm("Create Structure for Berlin"s, msg, std::format("{}\n\nactual position\n{}\nApplication: {}", 
			           details, MyPosition(), ApplicationText()));
	   }
   }


void TProcess::ImportBerlinOld(void) {
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