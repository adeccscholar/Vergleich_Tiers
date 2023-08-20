#include "Presenter_Process_Impl.h"

#include "UserDlg.h"

#include <adecc_Tools/MyExceptions.h>
#include <format>

/// Struktur zur Steuerung der Ausgabe
/// \todo später verschieben in zentrale Bibliothek
struct TMyNum : public std::numpunct<char> {
   char_type   do_decimal_point() const { return ','; }
   char_type   do_thousands_sep() const { return '.'; }
   string_type do_truename()      const { return "ja"; }
   string_type do_falsename()     const { return "nein"; }
   std::string do_grouping()      const { return "\3"; }
};

void TProcess_Presenter_Impl::InitMainForm(TMyForm&& form) {
   frm.swap(form);
   frm.SetCaption("Testanwendung Schichten ...");
   }

void TProcess_Presenter_Impl::SetMainFormCaption(std::string const& strCaption) {
   frm.SetCaption(strCaption);
   }

TMyForm TProcess_Presenter_Impl::CreateLoginForm(TMyForm& parent) {
   return TMyForm(new UserDlg(parent.Form()), true);
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


void TProcess_Presenter_Impl::ShowErrorForm(std::string const& caption, std::string const message) {
   frm.Message(EMyMessageType::error, caption, message);
   }

void TProcess_Presenter_Impl::ShowInformationForm(std::string const& caption, std::string const message) {
   frm.Message(EMyMessageType::information, caption, message);
   }