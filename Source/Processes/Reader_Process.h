#pragma once
#include <adecc_Tools/MyTrace.h>
#include <adecc_Tools/MyError.h>
#include <utility>
#include <expected>

#include "BaseOfProcesses.h"

using namespace std::string_literals;

class TMyCredential;

/// class with interface to common database functions
class TProcess_Reader : virtual public TOperationsBase {
   public:
      TProcess_Reader() : TOperationsBase() { Trace("constructor for TProcess_Reader called."s); }
      TProcess_Reader(TProcess_Reader const&) = delete;
      TProcess_Reader(TProcess_Reader&&) noexcept = delete;
      virtual ~TProcess_Reader() = default;

      TProcess_Reader& operator = (TProcess_Reader const&) = delete;
      TProcess_Reader& operator = (TProcess_Reader&&) noexcept = delete;

 
      /// @name Section with the methods representing general database functions
      /// \{
      [[nodiscard]] virtual bool IsConnectedToDatabase() const = 0;
      [[nodiscard]] virtual bool GetServerHasIntegratedSecurity() const = 0;
      [[nodiscard]] virtual std::pair<std::string, std::string> GetConnectionInformations() const = 0;
      [[nodiscard]] virtual std::string GetDatabaseInformations(void) const = 0;
      [[nodiscard]] virtual std::expected<std::string, MyErrorInfo> LoginToDb(TMyCredential&&) = 0;
      virtual void LogoutFromDb() = 0;
      
      //virtual void Test() = 0;
      /// \}
};
