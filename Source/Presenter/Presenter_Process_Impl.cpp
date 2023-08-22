﻿#include "Presenter_Process_Impl.h"
#include "MyFileDlg.h"
#include <adecc_Tools/MyExceptions.h>
#include <format>

std::vector<tplList<Narrow>> Berlin_Districts_Columns{
      tplList<Narrow> { "District",  190, EMyAlignmentType::left   },
      tplList<Narrow> { "Abbr.",      60, EMyAlignmentType::left   },
      tplList<Narrow> { "Code",       70, EMyAlignmentType::center },
      tplList<Narrow> { "City hall", 210, EMyAlignmentType::left   },
      tplList<Narrow> { "Zipcode",    70, EMyAlignmentType::left   },
      tplList<Narrow> { "City",      150, EMyAlignmentType::left   },
      tplList<Narrow> { "Street",    180, EMyAlignmentType::left   },
      tplList<Narrow> { "latitude",   80, EMyAlignmentType::right  },
      tplList<Narrow> { "longitude",  80, EMyAlignmentType::right  }
   };

/// Struktur zur Steuerung der Ausgabe
/// \todo später verschieben in zentrale Bibliothek
struct TMyNum : public std::numpunct<char> {
   char_type   do_decimal_point() const { return ','; }
   char_type   do_thousands_sep() const { return '.'; }
   string_type do_truename()      const { return "ja"; }
   string_type do_falsename()     const { return "nein"; }
   std::string do_grouping()      const { return "\3"; }
};

TMyNum newNumPunct;



void TProcess_Presenter_Impl::InitMainForm(TMyForm&& form, std::string const& strCaption) {
   frm.swap(form);
   try {
      std::ios_base::sync_with_stdio(false);
      myLoc = std::locale(std::locale("de_DE"), &newNumPunct);
      std::locale::global(myLoc);
      for (auto& s : { &std::cout, &std::cerr, &std::clog }) {
         s->imbue(myLoc);
         s->setf(std::ios::fixed);
         }

      frm.SetCaption(strCaption);
      frm.GetAsStream<Narrow, EMyFrameworkType::listview>(old_cout, "tblOutput", Berlin_Districts_Columns);
      frm.GetAsStream<Narrow, EMyFrameworkType::memo>(old_cerr, "textError");
      frm.GetAsStream<Narrow, EMyFrameworkType::statusbar>(old_clog, "sbMain");

      frm.Set<EMyFrameworkType::button, std::string>("btnLogin"s, "login ...");
      frm.Set<EMyFrameworkType::button, std::string>("btnImportBln"s, "migrate Berlin ...");
      frm.Enable<EMyFrameworkType::button>("btnLogin"s, true);
      } 
   catch (std::exception& ex) {
      frm.Message(EMyMessageType::information, "error while prepare application", ex.what());
      }
   std::clog << "application is successful started and ready\n";
   }

void TProcess_Presenter_Impl::SetMainFormCaption(std::string const& strCaption) {
   frm.SetCaption(strCaption);
   }


TProcess_Presenter_Impl::login_return TProcess_Presenter_Impl::LoginForm(std::string const& server, bool hasintegrated,
                                                                     bool integrated, std::string const& user) {
   TMyForm dlg(CreateLoginForm(frm));  // Diese Zeile in eigene

   dlg.SetCaption(server);
   dlg.Set<EMyFrameworkType::groupbox, std::string>("grpUser", "credentials");

   dlg.Set<EMyFrameworkType::label, std::string>("lblUser", "user:");
   dlg.Set<EMyFrameworkType::label, std::string>("lblPassword", "password:");
   dlg.Set<EMyFrameworkType::edit, std::string>("edtUser", user);
   dlg.Set<EMyFrameworkType::edit, std::string>("edtPassword", "");

   dlg.Set< EMyFrameworkType::checkbox, std::string>("chbIntegrated", "use integrated security");
   if (hasintegrated) {
      dlg.Visible< EMyFrameworkType::checkbox>("chbIntegrated", true);
      dlg.Set<EMyFrameworkType::checkbox, bool>("chbIntegrated", integrated);
      }
   else {
      dlg.Set<EMyFrameworkType::checkbox, bool>("chbIntegrated", false);
      dlg.Visible< EMyFrameworkType::checkbox>("chbIntegrated", false);
      }

   dlg.Set<EMyFrameworkType::button, std::string>("btnOk", "login");
   dlg.Set<EMyFrameworkType::button, std::string>("btnCancel", "cancel");

   if (dlg.ShowModal() == EMyRetResults::ok) {
      auto boIntegrated = hasintegrated ? dlg.Get<EMyFrameworkType::checkbox, bool>("chbIntegrated").value_or(false) : false;
      if (boIntegrated) return { ""s, ""s, boIntegrated };
      else {
         auto strUser = dlg.Get<EMyFrameworkType::edit, std::string>("edtUser");
         auto strPwd  = dlg.Get<EMyFrameworkType::edit, std::string>("edtPassword");

         if (strUser) return { *strUser, strPwd.value_or(""s), boIntegrated };
         else throw TMy_InputError("no username was entered in the login dialog.");
      }
   }
   else throw TMy_UserBreak(std::format("the user canceled the login process for the database \"{}\".", server));
   }

std::pair<bool, std::string> TProcess_Presenter_Impl::ChooseFile(std::string const& strInputFile) {
   auto [ret, strFile] = TMyFileDlg::SelectWithFileDirDlg(strInputFile);
   return { ret == EMyRetResults::ok, strFile };
   }

void TProcess_Presenter_Impl::ShowErrorForm(std::string const& caption, std::string const message) {
   //frm.Message(EMyMessageType::error, caption, message);
   TMyFileDlg::Message(EMyMessageType::error, caption, message);
   }

void TProcess_Presenter_Impl::ShowInformationForm(std::string const& caption, std::string const message) {
   //frm.Message(EMyMessageType::information, caption, message);
   TMyFileDlg::Message(EMyMessageType::information, caption, message);
}