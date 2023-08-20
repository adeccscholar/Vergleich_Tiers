#include "Reader_Process_Impl.h"
#include <adecc_Database/MyDatabaseExceptions.h>

template <my_db_credentials ty>
ty CreateTest(void) {
   if constexpr (std::is_same<ty, TMyMSSQL>::value) {
      TMyMSSQL database{ "GeoDaten" };
      return database;
      }
   else if constexpr (std::is_same<ty, TMyMySQL>::value) {
      TMyMySQL database{ "Geodaten", "root", "" };
      return database;
      }
   else if constexpr (std::is_same<ty, TMyOracle>::value) {
      // Setzen Sie den Pfad zur TNS-Datei
      qputenv("QOCI_CONFIG_FILE", "C:\\Oracle\\tnsnames.ora");

      // Setzen Sie den Datenbanktreiber auf QOCI oder QOCI8
      qputenv("QT_DATABASE_DRIVERS", "QOCI");

      TMyOracle database{ true, "XE", "system", "" };
      //TMyOracle database { "localhost", 1521, "XE", "system", "" };
      return database;
      }
   else if constexpr (std::is_same<ty, TMyInterbase>::value) {
      TMyInterbase database{ "D:\\Test\\Datenbank\\GEODATEN.GDB", "SYSDBA", "" };
      return database;
      }
   else if constexpr (std::is_same<ty, TMySQLite>::value) {
      TMySQLite database{ "d:\\test\\datenbank\\geodaten.sqlite" };
      return database;
      }
   }

TProcess_Reader_Impl::TProcess_Reader_Impl(void) : TProcess_Reader() {
   db = CreateTest<concrete_db_server>();
   }

bool TProcess_Reader_Impl::GetServerHasIntegratedSecurity(void) {
   return db.HasIntegratedSecurity();
   }

std::pair<std::string, std::string> TProcess_Reader_Impl::GetServerInformations(void) {
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