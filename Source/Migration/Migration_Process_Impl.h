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
};

using tmp = SubProcessConcrete<TProcess_Migration_Impl, "Test for Migration Subprocess", "1.0">;
//using TProcess_Migration_Test = concrete_process<tmp>;

