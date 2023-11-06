#pragma once

#if __has_include("MyTrace.h")
#include "MyTrace.h"
#elif __has_include("adecc_Tools/MyTrace.h")
#include "adecc_Tools/MyTrace.h"
#endif

#include <string>
#include <iostream>
#include <typeinfo>
#include <functional>
#include <format>

namespace test_space {
	
class TestSuite {
   private:
      std::ostream& out;
      unsigned int iErrors;
      unsigned int iWarnings;
      unsigned int iTests;
   public:
      TestSuite(std::string const& strMessage, std::ostream& para = std::cerr) : out(para), iTests(0u), iErrors(0u), iWarnings(0u) {
         out << std::format("{0:=>{1}}\n", "=", 80);
	      out << std::format("{:^{}}\n\n", strMessage, 80);
	      }
	
	   unsigned int Errors() const { return iErrors; }
      unsigned int Warnings() const { return iWarnings;  }
	
	   bool Finish() const {      
         out << std::endl << std::setfill('=') << std::setw(80) << std::right << "=" << std::endl
             << std::setfill(' ') << std::left << iTests << " test´cases executed" << std::endl;
         if (iErrors > 0) out << std::format("{} errors, testsuite failed", iErrors);
         else if (iWarnings > 0) out << std::format("testsuite successful, but with {} warnings", iWarnings);
              else out << "testsuite successful";
         out << std::endl;
         return iErrors == 0;
	      }
	
       void StartTest(std::string const& strMessage) {
          out << std::setfill('-') << std::setw(80) << std::right << "-" << std::endl;
          out << strMessage;
          out << std::setfill(' ') << std::left;
          }

       template <typename exception_ty = std::exception>
       bool Check_fn_exception(std::string const& strMessage, std::function<void()> fn) {
          ++iTests;
          try {
             try {
                StartTest(strMessage);
                fn();
                out << " - test failed, exception didn't raised";
	             ++iErrors;
                return false;
                }
             catch(exception_ty const& ex) {
                out << " - exception of type <" << typeid(exception_ty).name() << "> catched, test successfull" << std::endl << std::endl;
                try {
                   out << dynamic_cast<MySafety::TNumberError_Base const&>(ex).FullMessage();
                   }
                catch(std::bad_cast const& ex_cast) {
                   out << ex.what();
                   }
                out << std::endl << std::endl;
                }
             }
          catch (std::exception& ex) {
             out << " - std::exception catched, test successfull with warning" << std::endl << std::endl
                 << ex.what() << std::endl << std::endl;
             ++iWarnings;
             }
          return true;
          }

       bool Check_fn(std::string const& strMessage, std::function<void()> fn) {
          ++iTests;
          try {
             StartTest(strMessage);
             fn();
             out << " - test successful\n";
             return true;
             }
          catch (std::exception& ex) {
             out << " - exception raised, test missed\n\n" << ex.what() << "\n\n";
             }
          ++iErrors;
		    return false;
          }

       template <typename ty>
       bool Check_fn_equal(std::string const& strMessage, ty value, std::function<ty()> fn) {
          ++iTests;
          try {
             StartTest(strMessage);
             ty result = fn();
             if(result == value) {
                out << std::format(" - test successful, result = {}\n", result);
                return true;
                }
             else {
                out << std::format(" - test missed, result = {}, expected = {}\n", result, value);
                ++iErrors;
                return false;
                }
             }
          catch (std::exception& ex) {
             out << " - exception raised, test missed\n\n" << ex.what() << "\n\n";
             }
          ++iErrors;
          return false;
          }

   };

}  // end of namespace MySafety


