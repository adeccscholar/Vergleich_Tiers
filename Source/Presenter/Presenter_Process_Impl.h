#pragma once

#include "Processes/Presenter_Process.h"
#include "adecc_Scholar/MyForm.h"
#include "adecc_Scholar/MyStream.h"

#include <iostream>
#include <locale>


class  TProcess_Presenter_Impl : virtual public TProcess_Presenter {
private:
   TMyForm frm;  /// Hauptfester der Anwendung, wird in InitMainForm übernommen
   std::locale myLoc;

   static inline TStreamWrapper<Narrow> old_cout{ std::cout };
   static inline TStreamWrapper<Narrow> old_cerr{ std::cerr };
   static inline TStreamWrapper<Narrow> old_clog{ std::clog };

public:
   TProcess_Presenter_Impl(void);
   TProcess_Presenter_Impl(TProcess_Presenter_Impl const&) = delete;
   TProcess_Presenter_Impl(TProcess_Presenter_Impl&&) noexcept = delete;
   virtual ~TProcess_Presenter_Impl() = default;

   virtual void InitMainForm(TMyForm&& frm, std::string const& strCaption) override;
   virtual void SetMainFormCaption(std::string const&) override;
   virtual std::expected<login_return, MyErrorInfo> LoginForm(std::string const& server, bool hasintegrated, bool integrated, std::string const& user) override;
   virtual std::pair<bool, std::string> ChooseFile(std::string const&) override;
   virtual void ShowErrorForm(std::string const& caption, std::string const& message, std::string const& details) override;
   virtual void ShowInformationForm(std::string const& caption, std::string const& message, std::string const& details) override;
private:
   virtual TMyForm CreateLoginForm(TMyForm& parent) = 0;
};