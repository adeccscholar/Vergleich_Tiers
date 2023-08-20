#pragma once

#include "Process.h"
#include "Persistance/Reader_Process_Impl.h"

template <my_db_credentials credential_ty>
class TProcess_Impl : public TProcess<credential_ty>, public TProcess_Reader_Impl<credential_ty> {
};