#pragma once

#include "BusinessOperations.h"
#include "Presenter_Process.h"

#include "Reader_Create_Db.h"
#include "Reader_System_Db.h"
#include "Reader_Overview_Db.h"

#include "Migration_Process.h"
#include <adecc_Tools/MyTrace.h>

#include <type_traits>

#include <iostream>


/**
 * @brief summarized type as base class for TProcess. 
 * @details The abstract methods of the class TBusinessOperations is implemented here with the help of the methods of 
 * a pack of likewise abstract base classes. Partial inheritance is used for this purpose. 
 * @details These methods define subprocesses, which can be developed in different teams. 
*/
//using Process_Base = TProcess_Helper<TBusinessOperations, TProcess_Presenter, TProcess_Reader, TProcess_Migration>;
using Process_Base = TProcess_Helper<TBusinessOperations, TProcess_Presenter, TProcess_Reader_Create, TProcess_Reader_System, TProcess_Reader_Overview, TProcess_Migration>;


class TProcess : virtual public Process_Base {
   private:
		int iBuild  = 1001;
		bool boProcessIsInitialized = false;
   public:
		TProcess(void);
		virtual ~TProcess() = default;

		/// @name delete constructors and operators
		/// \{
		TProcess(TProcess const&) = delete;
		TProcess(TProcess&&) noexcept = delete;
		TProcess& operator = (TProcess const&) = delete;
		TProcess& operator = (TProcess&&) noexcept = delete;
		/// \}


		virtual std::string ApplicationVersion(void) const override;
		virtual std::string ApplicationText(void) const override;

		/**
		 * @name overridden abstact methods from TBusinessOperations
		 * \{
		*/
		virtual bool ReadyForOperations(void) const override { return boProcessIsInitialized && IsConnectedToDatabase(); }
		virtual std::string CaptionForMainForm(void) const override;
		virtual void Init(TMyForm&&) override;
		virtual void Close() override;
		virtual void Login(void) override;
		virtual void CreateStructureBlnOld(void) override;
		virtual void ImportBerlinOld(void) override;
	   /// \}
   };

