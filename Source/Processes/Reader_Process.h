#pragma once

#include <string>
#include "adecc_Database/MyDatabaseCredentials.h"

template <my_db_credentials credential_ty>
class TProcess_Reader {
   public:
      using db_credentials = credential_ty;
   protected:
      db_credentials credentials;
   public:

      TProcess_Reader() = default;
      TProcess_Reader(TProcess_Reader const& ref) { copy(ref); }
      
      TProcess_Reader(TProcess_Reader&& ref) noexcept { swap(ref); }

     virtual ~TProcess_Reader() = default;

      TProcess_Reader& operator = (TProcess_Reader const&) = default;
      TProcess_Reader& operator = (TProcess_Reader&&) noexcept = default;

      void swap(TProcess_Reader& ref) noexcept {
         swap(credentials, ref.credentials);
         }

      void copy(TProcess_Reader const& ref) {
         credentials = ref.credentials;
         }

      // Bereich mit den Teilprozessen

      virtual void ReadBerlinFromFile(std::string const&) = 0;

};
