#pragma once
#include <adecc_Tools/MyTrace.h>
#include <adecc_Tools/MyError.h>
#include <utility>
#include <expected>

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

 
      /// @name Section with the methods representing general database functions
      /// \{
      virtual bool IsConnectedToDatabase() const = 0;
      virtual bool GetServerHasIntegratedSecurity() const = 0;
      virtual std::pair<std::string, std::string> GetConnectionInformations() const = 0;
      virtual std::string GetDatabaseInformations(void) const = 0;
      virtual std::expected<std::string, MyErrorInfo> LoginToDb(TMyCredential&&) = 0;
      virtual void LogoutFromDb() = 0;

      /// \}
};
