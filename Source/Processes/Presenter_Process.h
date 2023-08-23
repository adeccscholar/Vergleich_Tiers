#pragma once

#include <adecc_Tools/MyError.h>

#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <expected>

class TMyForm;

/**
  \brief Klasse mit den Teilprozessen mit den aktiven Teilprozessen
*/
class TProcess_Presenter {
   public:
      using login_return = std::tuple<std::string, std::string, bool>;

      TProcess_Presenter() {  std::cerr << "constructor for TProcess_Presenter called\n"; }
      TProcess_Presenter(TProcess_Presenter const&) = delete;
      TProcess_Presenter(TProcess_Presenter&& ref) noexcept = delete;
      virtual ~TProcess_Presenter() = default;

      //
      virtual void InitMainForm(TMyForm&&, std::string const& strCaption) = 0;
      virtual void SetMainFormCaption(std::string const&) = 0;
      virtual std::expected<login_return, MyErrorInfo> LoginForm(std::string const& server, bool hasintegrated, bool integrated, std::string const& user) = 0;

      virtual std::pair<bool, std::string> ChooseFile(std::string const&) = 0;
      virtual void ShowErrorForm(std::string const& caption, std::string const message) = 0;
      virtual void ShowInformationForm(std::string const& caption, std::string const message) = 0;
};
