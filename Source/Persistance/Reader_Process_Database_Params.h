#pragma once

#include "adecc_Database/MyDatabaseCredentials.h"

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

