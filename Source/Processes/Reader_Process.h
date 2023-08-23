#pragma once

#include <iostream>
#include <string>
#include <utility>

class TMyCredential;

class TProcess_Reader {
   public:
      TProcess_Reader() { std::cerr << "constructor for TProcess_Reader called\n"; }
      TProcess_Reader(TProcess_Reader const&) = delete;
      TProcess_Reader(TProcess_Reader&&) noexcept = delete;
      virtual ~TProcess_Reader() = default;

      // Bereich mit den Teilprozessen
      virtual bool GetServerHasIntegratedSecurity() = 0;
      virtual std::pair<std::string, std::string> GetConnectionInformations() = 0;
      virtual std::pair<bool, std::string> LoginToDb(TMyCredential&&) = 0;


      //virtual void ReadBerlinFromFile(std::string const&) = 0;

};
