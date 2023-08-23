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

	   virtual bool GetServerHasIntegratedSecurity(void) override;
	   virtual std::pair<std::string, std::string> GetConnectionInformations(void) override;
	   virtual std::pair<bool, std::string> LoginToDb(TMyCredential&& credentials) override;
	   
		void Check() {	delete new TProcess_Reader_Impl; }
   };

