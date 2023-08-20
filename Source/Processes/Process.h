﻿#pragma once

#include "Presenter_Process.h"
#include "Reader_Process.h"
#include "adecc_Tools/MyExceptions.h"
#include "BusinessOperations.h"
#include <string>
#include <tuple>


class TProcess : virtual public TBusinessOperations, virtual public TProcess_Presenter, virtual public TProcess_Reader {
	public:
		TProcess(void);
		TProcess(TProcess const& ref);
		TProcess(TProcess&& ref) noexcept;

	   virtual ~TProcess() { }
	   
		void swap(TProcess& ref) noexcept;
		void copy(TProcess const& ref);

		virtual void Login(void) override;


	
   };
