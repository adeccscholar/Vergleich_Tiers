#pragma once

#include "BaseOfProcesses.h"
#include <string>
using namespace std::string_literals;

class TMyForm;

/**
 * \brief Interface for the business processes themselves, which are processed by the TProcess class with the help of the subprocess
*/
class TBusinessOperations : virtual public TOperationsBase  {
   private:
		std::string strApplication = "GeoDatenApp"s;  ///< Name of the application for which the business processes are provided
		int         iMajorVersion  = 1;               ///< Major version number for the application / business processes
		int         iMinorVersion  = 0;               ///< Minor version number for the application / business processes
	public:
		TBusinessOperations();
		TBusinessOperations(TBusinessOperations const&) = delete;
		TBusinessOperations(TBusinessOperations&&) noexcept = delete;
		virtual ~TBusinessOperations(void);

		TBusinessOperations& operator = (TBusinessOperations const&) = delete;
		TBusinessOperations& operator = (TBusinessOperations&&) noexcept = delete;

		virtual std::string Application(void) const override;
		virtual std::string ApplicationVersion(void) const override;
		virtual std::string ApplicationText(void) const override;
		virtual std::string CaptionForMainForm(void) const = 0;

		/// process is initialized and connected, ready to proceed operations
		virtual bool ReadyForOperations(void) const = 0;
		/// initialize the application, processes and connection to the main window
		virtual void Init(TMyForm&&) = 0;
      /// close and finishing the process
		virtual void Close() = 0;
		/// Dialog to enter the user / password and connect to the database
		virtual void Login(void) = 0;
		/// Creating the structure for Berlin (old) in database 
		virtual void CreateStructureBlnOld(void) = 0;
		/// Reading the file with the open data information for Berlin and importing them into the database
		virtual void ImportBerlinOld(void) = 0;

		// virtual void Test(void) = 0;
   };
