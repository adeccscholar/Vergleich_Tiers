#pragma once

#include "Processes/Reader_Process.h"
#include "adecc_Database/MyDatabase.h"

template <my_db_credentials credential_ty>
class TProcess_Reader_Impl : virtual public TProcess_Reader<credential_ty> {
   public:
		using concrete_db_server     = credential_ty; // TMyMSSQL; // TMyMSSQL TMyMySQL TMyOracle TMyInterbase TMySQLite
		using concrete_framework     = TMyQtDb<concrete_db_server>;
		using concrete_db_connection = TMyDatabase<TMyQtDb, concrete_db_server>;
		using concrete_query         = TMyQuery<TMyQtDb, concrete_db_server>;
   private:
		concrete_db_connection db { };
	public:
	TProcess_Reader_Impl() : TProcess_Reader<credential_ty>() { }
	TProcess_Reader_Impl(TProcess_Reader_Impl const& ref) : TProcess_Reader<credential_ty>(ref) { }
	


	virtual void ReadBerlinFromFile(std::string const& strFile) override { };
   };

