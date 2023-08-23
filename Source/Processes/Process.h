#pragma once

#include "Presenter_Process.h"
#include "Reader_Process.h"
#include "Migration_Process.h"
#include "BusinessOperations.h"

#include <iostream>

template <typename proc_ty, typename... proc_impl_types>
class TProcess_Helper : virtual public proc_ty, virtual public proc_impl_types... {
public:
	TProcess_Helper() : proc_ty(), proc_impl_types()... { std::cerr << "constructor process_helper called\n"; }

	TProcess_Helper(TProcess_Helper const& ref) = delete;      //: proc_ty(ref), proc_impl_types(ref)... { }
	TProcess_Helper(TProcess_Helper&& rref) noexcept = delete; //: proc_ty(std::move(rref)), proc_impl_types(std::move(rref))... { }
	virtual ~TProcess_Helper() = default;
   };


/*
class TProcess : virtual public TBusinessOperations, virtual public TProcess_Presenter, 
	              virtual public TProcess_Reader, virtual public TProcess_Migration {
*/

using Process_Base = TProcess_Helper<TBusinessOperations, TProcess_Presenter, TProcess_Reader, TProcess_Migration>;
class TProcess : virtual public Process_Base {
   private:
		//using base_class = TProcess_Helper<TBusinessOperations, TProcess_Presenter, TProcess_Reader, TProcess_Migration>;
	   int iBuild;
   public:
		TProcess(void);
		TProcess(TProcess const&) = delete;
		TProcess(TProcess&&) noexcept = delete;
		virtual ~TProcess() = default;
	   
		virtual std::string ApplicationText(void) override;

		// Methoden aus TBusinessOperations mit Hilfe der Methoden aus den anderen Klassen implementieren
		virtual void Init(TMyForm&&) override;
		virtual void Login(void) override;
		virtual void ImportBerlin(void) override;


	
   };

