#include "Reader_Process_Impl.h"
#include <adecc_Database/MyDatabaseExceptions.h>

#include "Reader_Process_Database_Params.h"

TProcess_Reader_Impl::TProcess_Reader_Impl(void) : TProcess_Reader() {
   db = CreateTest<concrete_db_server>();
   Trace("constructor for TProcess_Reader_Impl called");
   }


bool TProcess_Reader_Impl::GetServerHasIntegratedSecurity(void) const {
   return db.HasIntegratedSecurity();
   }

std::pair<std::string, std::string> TProcess_Reader_Impl::GetConnectionInformations(void) const {
   return { db.GetDatabase(), db.ServerType() };
   }

std::string TProcess_Reader_Impl::GetDatabaseInformations(void) const {
   return db.GetInformations();
}

std::expected<std::string, MyErrorInfo> TProcess_Reader_Impl::LoginToDb(TMyCredential&& credentials) {
   try {
      db += credentials;
      db.Open();
      return { db.GetInformations() };
      }
   catch (TMy_Db_Exception& ex) {
      return std::unexpected( MyErrorInfo { EMyErrorType::DatabaseError, "error while login to database"s, ex.information() } );
      }
   catch (std::exception& ex) {
      return std::unexpected(MyErrorInfo{ EMyErrorType::DatabaseError, "error while login to database"s, ex.what() });
      }
   }