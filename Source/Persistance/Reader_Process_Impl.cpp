#include "Reader_Process_Impl.h"
#include <adecc_Database/MyDatabaseExceptions.h>

#include "Reader_Process_Database_Params.h"

TProcess_Reader_Impl::TProcess_Reader_Impl(void) : TProcess_Reader() {
   db = CreateTest<concrete_db_server>();
   }

bool TProcess_Reader_Impl::GetServerHasIntegratedSecurity(void) {
   return db.HasIntegratedSecurity();
   }

std::pair<std::string, std::string> TProcess_Reader_Impl::GetConnectionInformations(void) {
   return { db.GetDatabase(), db.ServerType() };
   }


std::pair<bool, std::string> TProcess_Reader_Impl::LoginToDb(TMyCredential&& credentials) {
   try {
      db += credentials;
      db.Open();
      return { true, db.GetInformations() };
      }
   catch (TMy_Db_Exception& ex) {
      return { false, ex.information() };
      }
   catch (std::exception& ex) {
      return { false, ex.what() };
      }
   }