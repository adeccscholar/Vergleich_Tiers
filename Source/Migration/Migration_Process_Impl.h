#pragma once

#include "Processes/Migration_Process.h"



class TProcess_Migration_Impl : virtual public TProcess_Migration {

public:
	TProcess_Migration_Impl();
	TProcess_Migration_Impl(TProcess_Migration_Impl const&) = delete;
	TProcess_Migration_Impl(TProcess_Migration_Impl&&) = delete;
	virtual ~TProcess_Migration_Impl() = default;
	// RValue Konstruktor, Destruktor, swap

	virtual void ReadBerlinFromFile(std::string const& strFile) override;

	void Check() { delete new TProcess_Migration_Impl; }
};

