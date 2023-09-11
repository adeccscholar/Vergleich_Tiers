#pragma once

#include "Processes/Reader_Process.h"
#include "adecc_Database/MyDatabase.h"

#include <vector>
#include <utility>

//template <my_db_credentials credential_ty>
class TProcess_Reader_Impl : virtual public TProcess_Reader {
   public:
		//using concrete_db_server = credential_ty;
		using concrete_db_server     = TMyMSSQL; // TMyMSSQL TMyMySQL TMyOracle TMyInterbase TMySQLite
		using concrete_framework     = TMyQtDb<concrete_db_server>;
		using concrete_db_connection = TMyDatabase<TMyQtDb, concrete_db_server>;
		using concrete_query         = TMyQuery<TMyQtDb, concrete_db_server>;

		enum class EEntity_type : int { unknow, table, view };
		using entity_type_set = std::vector<std::pair<EEntity_type, std::string>>;

   private:
		concrete_db_connection db { };
	public:
		TProcess_Reader_Impl();
		TProcess_Reader_Impl(TProcess_Reader_Impl const&) = delete;
		TProcess_Reader_Impl(TProcess_Reader_Impl&&) noexcept = delete;
		virtual ~TProcess_Reader_Impl() = default;

		concrete_db_connection& Database() { return db; }
		concrete_db_connection const& Database() const { return db; }

		[[nodiscard]] virtual bool IsConnectedToDatabase() const {return db.Connected(); }
		[[nodiscard]] virtual bool GetServerHasIntegratedSecurity(void) const override;
		[[nodiscard]] virtual std::pair<std::string, std::string> GetConnectionInformations(void) const override;
		[[nodiscard]] virtual std::string GetDatabaseInformations(void) const override;
		[[nodiscard]] virtual std::expected<std::string, MyErrorInfo> LoginToDb(TMyCredential&& credentials) override;
		virtual void LogoutFromDb(void) override;
	   
		//virtual void Test() = 0;
   };

using TProcess_Reader_Impl_Test = concrete_process<SubProcessConcrete<TProcess_Reader_Impl, "Test_Db_Imp", "1.0">>;
