#include "Reader_Create_Db_Impl.h"
#include "Reader_Process_Impl.h"

#include <sstream>

const TProcess_Reader_Impl::entity_type_set TProcess_Reader_Create_Impl::structure_info_berlin_old {
   { TProcess_Reader_Impl::EEntity_type::view,  "vw_Berlin_Streets_Districts"s },
   { TProcess_Reader_Impl::EEntity_type::view,  "vw_Berlin_Streets_Neighborhoods"s },
   { TProcess_Reader_Impl::EEntity_type::view,  "vw_Berlin_Streets"s },
   { TProcess_Reader_Impl::EEntity_type::view,  "vw_Berlin_Neighborhoods"s },
   { TProcess_Reader_Impl::EEntity_type::view,  "vw_Berlin_Districts"s },
   { TProcess_Reader_Impl::EEntity_type::table, "Berlin_Neighborhoods"s },
   { TProcess_Reader_Impl::EEntity_type::table, "Berlin_Districts" },
   { TProcess_Reader_Impl::EEntity_type::table, "Berlin"s } };

const std::vector<std::string>  TProcess_Reader_Create_Impl::create_statements_berlin_old {
     "CREATE TABLE Berlin (\n"s +
     "   State_ID         INTEGER,\n"s +
     "   District_ID      INTEGER,\n"s +
     "   Neighborhood_ID  INTEGER,\n"s +
     "   Street_ID        INTEGER,\n"s +
     "   State            VARCHAR(75),\n"s +
     "   City             VARCHAR(75),\n"s +
     "   District         VARCHAR(75),\n"s +
     "   Neighborhood     VARCHAR(75),\n"s +
     "   Street           VARCHAR(75),\n"s +
     "   Postal_Code      VARCHAR(5),\n"s +
     "   House_Number     INTEGER,\n"s +
     "   Address_Addendum VARCHAR(5),\n"s +
     "   Latitude         FLOAT ,\n"s +
     "   Longitude        FLOAT ,\n"s +
     "   Address_Date     DATE ,\n"s +
     "   Street_Date      DATE ,\n"s +
     "   Quality          VARCHAR(100) ,\n"s +
     "   Address_Type     VARCHAR(100) ,\n"s +
     "   HKO_ID           VARCHAR(20) \n"s +
     "   )"s,

     "CREATE TABLE Berlin_Districts ("s +
     "   District                       VARCHAR(75),"s +
     "   Abbreviation                   VARCHAR(10),"s +
     "   State_ID                       INTEGER,"s +
     "   District_ID                    INTEGER,"s +
     "   Latitude_Geographic_Center     FLOAT,"s +
     "   Longitude_Geographic_Center    FLOAT,"s +
     "   City_Hall                      VARCHAR(75),"s +
     "   City_Hall_Address_Postal_Code  VARCHAR(5),"s +
     "   City_Hall_Address_City         VARCHAR(75),"s +
     "   City_Hall_Address_Street       VARCHAR(75),"s +
     "   City_Hall_Address_House_Number VARCHAR(20),"s +
     "   City_Hall_Address_Latitude     FLOAT,"s +
     "   City_Hall_Address_Longitude    FLOAT"s +
     "   )"s,

     "CREATE TABLE Berlin_Neighborhoods( "s +
     "   State_ID                    INTEGER,"s +
     "   District_ID                 INTEGER,"s +
     "   Neighborhood_ID             INTEGER,"s +
     "   District                    VARCHAR(75),"s +
     "   Neighborhood                VARCHAR(75),"s +
     "   Latitude_Geographic_Center  FLOAT,"s +
     "   Longitude_Geographic_Center FLOAT"s +
     "   )"s,

     "CREATE VIEW vw_Berlin_Districts\n"s +
     "AS\n"s +
     "   SELECT DISTINCT State_ID, District_ID, State, City, District\n"s +
     "   FROM Berlin"s,

     "CREATE VIEW vw_Berlin_Neighborhoods\n"s +
     "AS\n"s +
     "  SELECT DISTINCT State_ID, District_ID, Neighborhood_ID, State, City, District, Neighborhood\n"s +
     "  FROM Berlin"s,

     "CREATE VIEW vw_Berlin_Streets_Neighborhoods\n"s +
     "AS\n"s +
     "  SELECT DISTINCT State_ID, District_ID,  Neighborhood_ID, Street_ID, State, City, District, Neighborhood, Street\n"s +
     "  FROM Berlin"s,

     "CREATE VIEW vw_Berlin_Streets_Districts\n"s +
     "AS\n"s +
     "  SELECT DISTINCT State_ID, District_ID, Street_ID, State, City, District, Street\n"s +
     "  FROM Berlin"s,

     "CREATE VIEW vw_Berlin_Streets\n"s +
     "AS\n"s +
     "  SELECT DISTINCT State_ID, Street_ID, State, Street\n"s +
     "  FROM Berlin"s

};




TProcess_Reader_Create_Impl::TProcess_Reader_Create_Impl() : TProcess_Reader_Create(), TProcess_Reader_Impl() { 
   Trace("constructor for TProcess_Reader_Create_Impl called."s); 
   }

/**
 * @brief Check database for tables and views associated to data for Berlin (old model)
 * @return std::expected<std::pair<bool, std::string>, MyErrorInfo> when processed pair with bool and string, otherwise MyErrorInfo
*/
std::expected<std::pair<bool, std::string>, MyErrorInfo> TProcess_Reader_Create_Impl::HasDbStructureForBerlin() const {

   if(Database().Connected()) {
      try {
         auto allViews = Database().GetViewNames("");
         auto allTables = Database().GetTableNames("");
         std::ostringstream os_entries;
         size_t iCount = 0;
         for (auto [etype, name] : structure_info_berlin_old) {
            if constexpr (std::is_same<concrete_db_server, TMyOracle>::value || std::is_same<concrete_db_server, TMyInterbase>::value) {
               std::transform(name.begin(), name.end(), name.begin(), [](auto c) { return std::toupper(c);  });
               }
            else if constexpr (std::is_same<concrete_db_server, TMyMySQL>::value) {
               std::transform(name.begin(), name.end(), name.begin(), [](auto c) { return std::tolower(c);  });
               }

            switch (etype) {
               case EEntity_type::view:
                  if (allViews.find(name) != allViews.end()) {
                     os_entries << std::format("{}: VIEW {}\n", ++iCount, name);
                     }
                  break;
               case EEntity_type::table:
                  if (allTables.find(name) != allTables.end()) {
                     os_entries << std::format("{}: TABLE {}\n", ++iCount, name);
                     }
                  break;
               default:
                  return std::unexpected( MyErrorInfo { EMyErrorType::DatabaseError, "unexpected entity type in cleanup", "" } );
               }

            }
         return { { iCount > 0, os_entries.str() } };
         }
      catch(std::runtime_error const& ex) {
         return std::unexpected(MyErrorInfo { EMyErrorType::DatabaseError, "create aborted", ex.what() } );
         }
      } 
   else {
      auto strMsg = "Database not open, please contact the support with the details"s;
      auto strDetail = std::format("Database isn't open for requested operation.\n"
                                   "Operation: Create structure for Berlin (old)\n"
                                   "Database informations: {}\n\n{}\n{}", Database().GetInformations(), MyTimeStamp(), MyPosition());
      return std::unexpected(MyErrorInfo{ EMyErrorType::RuntimeError, strMsg, strDetail });
      }
   }

std::expected<bool, MyErrorInfo> TProcess_Reader_Create_Impl::DropDbStructureForBerlin() {

   if (Database().Connected()) {
      try {
         auto allViews = Database().GetViewNames("");
         auto allTables = Database().GetTableNames("");

         auto query = Database().CreateQuery();

         for (auto [etype, name] : structure_info_berlin_old) {
            if constexpr (std::is_same<concrete_db_server, TMyOracle>::value || std::is_same<concrete_db_server, TMyInterbase>::value) {
               std::transform(name.begin(), name.end(), name.begin(), [](auto c) { return std::toupper(c);  });
            }
            else if constexpr (std::is_same<concrete_db_server, TMyMySQL>::value) {
               std::transform(name.begin(), name.end(), name.begin(), [](auto c) { return std::tolower(c);  });
            }

            bool boExecute = false;

            switch (etype) {
            case EEntity_type::view:
               if (boExecute = allViews.find(name) != allViews.end(); boExecute) query.SetSQL(std::format("DROP VIEW {}", name));
               break;
            case EEntity_type::table:
               if (boExecute = allTables.find(name) != allTables.end(); boExecute) query.SetSQL(std::format("DROP TABLE {}", name));
               break;
            default:
               return std::unexpected(MyErrorInfo{ EMyErrorType::DatabaseError, "unexpected entity type in cleanup", "" });
            }

            query.Execute();
            }
         }
      catch (TMy_Db_Exception const& ex) {
         return std::unexpected(MyErrorInfo{ EMyErrorType::DatabaseError, "operations aborted", ex.information() });
         }
      catch (std::runtime_error const& ex) {
         return std::unexpected(MyErrorInfo{ EMyErrorType::DatabaseError, "create aborted", ex.what() });
      }
   }
   else {
      auto strMsg = "Database not open, please contact the support with the details"s;
      auto strDetail = std::format("Database isn't open for requested operation.\n"
         "Operation: Create structure for Berlin (old)\n"
         "Database informations: {}\n\n{}\n{}", Database().GetInformations(), MyTimeStamp(), MyPosition());
      return std::unexpected(MyErrorInfo{ EMyErrorType::RuntimeError, strMsg, strDetail });
      }
   return { true };
   }

std::expected<bool, MyErrorInfo> TProcess_Reader_Create_Impl::CreateDbStructureForBerlin() {
   try {
      if (Database().Connected()) {
         auto query = Database().CreateQuery();
         for (auto const& stmt : create_statements_berlin_old) {
            query.SetSQL(stmt);
            query.Execute();
            }
         return { true };
         }
      else {
         auto strMsg = "Database not open, please contact the support with the details"s;
         auto strDetail = std::format("Database isn't open for requested operation.\n"
            "Operation: Create structure for Berlin (old)\n"
            "Database informations: {}\n\n{}\n{}", Database().GetInformations(), MyTimeStamp(), MyPosition());
         return std::unexpected(MyErrorInfo{ EMyErrorType::RuntimeError, strMsg, strDetail });
         }
      }
   catch(TMy_Db_Exception const& ex) {
      return std::unexpected(MyErrorInfo{ EMyErrorType::DatabaseError, "operations aborted", ex.information() } );
      }
   catch(std::exception const& ex) {
      return std::unexpected(MyErrorInfo{ EMyErrorType::DatabaseError, "operations aborted", ex.what() });
      }
   }
