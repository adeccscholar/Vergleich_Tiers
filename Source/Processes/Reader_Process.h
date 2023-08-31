#pragma once
#include <adecc_Tools/MyTrace.h>
#include <utility>

using namespace std::string_literals;

class TMyCredential;

class TProcess_Reader {
   public:
      TProcess_Reader() { Trace("constructor for TProcess_Reader called."s); }
      TProcess_Reader(TProcess_Reader const&) = delete;
      TProcess_Reader(TProcess_Reader&&) noexcept = delete;
      virtual ~TProcess_Reader() = default;

      TProcess_Reader& operator = (TProcess_Reader const&) = delete;
      TProcess_Reader& operator = (TProcess_Reader&&) noexcept = delete;

      // Bereich mit den Teilprozessen
      virtual bool GetServerHasIntegratedSecurity() = 0;
      virtual std::pair<std::string, std::string> GetConnectionInformations() = 0;
      virtual std::pair<bool, std::string> LoginToDb(TMyCredential&&) = 0;


};
