#pragma once

#include "Processes/Reader_Process.h"
#include "adecc_Database/MyDatabase.h"

//template <my_db_credentials credential_ty>
class TProcess_Reader_Impl : virtual public TProcess_Reader {
   public:
		//using concrete_db_server = credential_ty;
		using concrete_db_server     = TMyMSSQL; // TMyMSSQL TMyMySQL TMyOracle TMyInterbase TMySQLite
		using concrete_framework     = TMyQtDb<concrete_db_server>;
		using concrete_db_connection = TMyDatabase<TMyQtDb, concrete_db_server>;
		using concrete_query         = TMyQuery<TMyQtDb, concrete_db_server>;

   private:
		concrete_db_connection db { };
	public:
		TProcess_Reader_Impl();
		TProcess_Reader_Impl(TProcess_Reader_Impl const&) = delete;
		TProcess_Reader_Impl(TProcess_Reader_Impl&&) noexcept = delete;
		virtual ~TProcess_Reader_Impl() = default;

		virtual bool IsConnectedToDatabase() const {return db.Connected(); }
	   virtual bool GetServerHasIntegratedSecurity(void) const override;
	   virtual std::pair<std::string, std::string> GetConnectionInformations(void) const override;
		virtual std::string GetDatabaseInformations(void) const override;
	   virtual std::expected<std::string, MyErrorInfo> LoginToDb(TMyCredential&& credentials) override;
		virtual void LogoutFromDb(void) override;
	   
		void Check() {	delete new TProcess_Reader_Impl; }
   };
