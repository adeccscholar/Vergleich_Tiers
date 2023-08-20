#pragma once

#include "Presenter_Process.h"
#include "Reader_Process.h"

/**
 * \brief Interface für die eigentlichen Geschäftsvorfälle, die der Klasse TProcess mit Hilfe der Teilprozesse verarbeitet werden
*/
class TBusinessOperations {
	public:
	   TBusinessOperations(void) = default;
	   TBusinessOperations(TBusinessOperations const&) = default;
	   virtual ~TBusinessOperations(void) = default;

		void swap(TBusinessOperations& ref) noexcept { }
		void copy(TBusinessOperations const& ref) { }
   };



template <my_db_credentials credential_ty>
class TProcess : virtual public TBusinessOperations, virtual public TProcess_Presenter, virtual public TProcess_Reader<credential_ty> {
	public:
	   TProcess() : TBusinessOperations(), TProcess_Presenter(), TProcess_Reader<credential_ty>() { }
	   TProcess(TProcess const& ref) : TBusinessOperations(ref), TProcess_Presenter(ref), TProcess_Reader<credential_ty>(ref) { }
	   
	   virtual ~TProcess() { }
	   
		void swap(TProcess& ref) noexcept {
			TBusinessOperations::swap(static_cast<TBusinessOperations&>(ref));
			TProcess_Presenter::swap(static_cast<TProcess_Presenter&>(ref));
			TProcess_Reader::swap(static_cast<TProcess_Reader&>(ref));
		   }

		void copy(TProcess const& ref) { 
			TBusinessOperations::copy(static_cast<TBusinessOperations const&>(ref));
			TProcess_Presenter::copy(static_cast<TProcess_Presenter const&>(ref));
			TProcess_Reader::copy(static_cast<TProcess_Reader const&>(ref));
		   }
};
