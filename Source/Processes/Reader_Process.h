#pragma once

#include <string>
#include <utility>

class TMyCredential;

class TProcess_Reader {
   public:
      TProcess_Reader() = default;
      TProcess_Reader(TProcess_Reader const& ref) { copy(ref); }
      TProcess_Reader(TProcess_Reader&& ref) noexcept { swap(ref); }

      virtual ~TProcess_Reader() = default;

       void swap(TProcess_Reader& ref) noexcept {
         }

      void copy(TProcess_Reader const& ref) {
         }

      // Bereich mit den Teilprozessen
      virtual bool GetServerHasIntegratedSecurity() = 0;
      virtual std::pair<std::string, std::string> GetConnectionInformations() = 0;
      virtual std::pair<bool, std::string> LoginToDb(TMyCredential&&) = 0;


      //virtual void ReadBerlinFromFile(std::string const&) = 0;

};
