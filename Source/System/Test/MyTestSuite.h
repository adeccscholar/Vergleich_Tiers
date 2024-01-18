#pragma once

#if __has_include("MyTrace.h")
#include "MyTrace.h"
#elif __has_include("adecc_Tools/MyTrace.h")
#include "adecc_Tools/MyTrace.h"
#endif

#include <string>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <functional>
#include <format>

namespace test_space {
	// R A I I
   class redirect_to_string_stream {
      private:
         std::ostream& oldStream;
         std::streambuf* old_stream_buff;
         std::ostringstream stringStream;
         //bool is_redirected;
      public:
         redirect_to_string_stream(std::ostream& old_out = std::cout) 
                  : oldStream(old_out), old_stream_buff(nullptr) {
            old_stream_buff = oldStream.rdbuf(stringStream.rdbuf());
            }

         ~redirect_to_string_stream() {
            finish_redirection();
            }

         void finish_redirection(void) {
            if (old_stream_buff != nullptr) oldStream.rdbuf(old_stream_buff);
            old_stream_buff = nullptr;
            }

         void print_content() {
            if(old_stream_buff == nullptr) {
               oldStream << stringStream.str() << std::endl;
               }
            }
      };

class TestSuite {
   private:
      std::ostream& out;
      unsigned int iErrors;
      unsigned int iWarnings;
      unsigned int iTests;
   public:
      TestSuite(std::string const& strMessage, std::ostream& para = std::cerr) : out(para), iTests(0u), iErrors(0u), iWarnings(0u) {
         out << std::format("{0:=>{1}}\n", "=", 120);
	      out << std::format("{:^{}}\n\n", strMessage, 120);
         out << std::format("{0:=>{1}}\n\n", "=", 120);
	      }
	
	   unsigned int Errors() const { return iErrors; }
      unsigned int Warnings() const { return iWarnings;  }
	
	   bool Finish() const {      
         out << std::endl << std::setfill('=') << std::setw(120) << std::right << "=" << std::endl
             << std::setfill(' ') << std::left << iTests << " test´cases executed" << std::endl;
         if (iErrors > 0) out << std::format("{} errors, testsuite failed", iErrors);
         else if (iWarnings > 0) out << std::format("testsuite successful, but with {} warnings", iWarnings);
              else out << "testsuite successful";
         out << std::endl;
         return iErrors == 0;
	      }
	
       void StartTest(std::string const& strMessage) {
          out << std::setfill('-') << std::setw(120) << std::right << "-" << std::endl;
          out << strMessage;
          out << std::setfill(' ') << std::left;
          }

       template <typename exception_ty = std::exception>
       bool Check_fn_exception(std::string const& strMessage, std::function<void()> fn) {
          StartTest(strMessage);
          redirect_to_string_stream redirect(out);
          ++iTests;
          try {
             try {
                fn();
                redirect.finish_redirection();
                out << " - test failed, exception didn't raised";
                redirect.print_content();
	             ++iErrors;
                return false;
                }
             catch(exception_ty const& ex) {
                redirect.finish_redirection();
                out << " - exception of type <" << typeid(exception_ty).name() << "> catched, test successfull" << std::endl << std::endl;
                try {
                   out << dynamic_cast<MySafety::TNumberError_Base const&>(ex).FullMessage();
                   }
                catch(std::bad_cast const& ex_cast) {
                   out << ex.what();
                   }
                out << std::endl << std::endl << "Output:\n";
                redirect.print_content();
                }
             }
          catch (std::exception& ex) {
             redirect.finish_redirection();
             out << " - std::exception catched, test successfull with warning" << std::endl << std::endl
                 << ex.what() << std::endl << std::endl << "Output:\n";
             redirect.print_content();
             ++iWarnings;
             }
          return true;
          }

       bool Check_fn(std::string const& strMessage, std::function<void()> fn) {
          StartTest(strMessage);
          redirect_to_string_stream redirect(out);
          ++iTests;
          try {
             fn();
             redirect.finish_redirection();
             out << " - test successful\n\nOutput:\n";
             redirect.print_content();
             return true;
             }
          catch (std::exception& ex) {
             redirect.finish_redirection();
             out << " - exception raised, test missed\n" << ex.what() << "\n\nOutput:\n";
             redirect.print_content();
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


