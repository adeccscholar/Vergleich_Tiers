#pragma once

#include "adecc_Scholar/MyForm.h"

/**
  \brief Klasse mit den Teilprozessen mit den aktiven Teilprozessen
*/
class TProcess_Presenter {
   public:
      using login_return = std::tuple<std::string, std::string, bool>;

      TProcess_Presenter() = default;
      TProcess_Presenter(TProcess_Presenter const&) = default;
      TProcess_Presenter(TProcess_Presenter&& ref) noexcept { swap(ref); }

     virtual ~TProcess_Presenter() = default;

      TProcess_Presenter& operator =(TProcess_Presenter const&) = default;
      TProcess_Presenter& operator =(TProcess_Presenter&&) noexcept = default;

      void swap(TProcess_Presenter &ref) noexcept { }
      void copy(TProcess_Presenter const& ref) { }

      //
      virtual void InitMainForm(TMyForm&&, std::string const& strCaption) = 0;
      virtual void SetMainFormCaption(std::string const&) = 0;
      virtual login_return LoginForm(std::string const& server, bool hasintegrated, bool integrated, std::string const& user) = 0;

      virtual void ShowErrorForm(std::string const& caption, std::string const message) = 0;
      virtual void ShowInformationForm(std::string const& caption, std::string const message) = 0;
};
