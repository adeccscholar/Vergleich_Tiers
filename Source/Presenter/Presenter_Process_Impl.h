﻿#pragma once

#include "Processes/Presenter_Process.h"
#include "adecc_Scholar/MyForm.h"
#include "adecc_Scholar/MyStream.h"

#include <locale>


class  TProcess_Presenter_Impl : virtual public TProcess_Presenter {
private:
   TMyForm frm;  /// Hauptfester der Anwendung, wird in InitMainForm übernommen
   std::locale myLoc;

   static inline TStreamWrapper<Narrow> old_cout{ std::cout };
   static inline TStreamWrapper<Narrow> old_cerr{ std::cerr };
   static inline TStreamWrapper<Narrow> old_clog{ std::clog };

public:
   TProcess_Presenter_Impl(void) : TProcess_Presenter() { }
   TProcess_Presenter_Impl(TProcess_Presenter_Impl const& ref) : TProcess_Presenter(ref) { }
   virtual ~TProcess_Presenter_Impl() { }

   virtual void InitMainForm(TMyForm&& frm, std::string const& strCaption) override;
   virtual void SetMainFormCaption(std::string const&) override;
   virtual login_return LoginForm(std::string const& server, bool hasintegrated, bool integrated, std::string const& user) override;
   virtual std::pair<bool, std::string> ChooseFile(std::string const&) override;
   virtual void ShowErrorForm(std::string const& caption, std::string const message) override;
   virtual void ShowInformationForm(std::string const& caption, std::string const message) override;
private:
   virtual TMyForm CreateLoginForm(TMyForm& parent) = 0;
};