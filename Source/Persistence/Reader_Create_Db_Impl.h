#pragma once

#include "Processes/Reader_Create_Db.h"
#include "Reader_Process_Impl.h"

class TProcess_Reader_Create_Impl : virtual public TProcess_Reader_Create, virtual public TProcess_Reader_Impl {
   public:
      TProcess_Reader_Create_Impl();
      TProcess_Reader_Create_Impl(TProcess_Reader_Create_Impl const&) = delete;
      TProcess_Reader_Create_Impl(TProcess_Reader_Create_Impl&&) noexcept = delete;
      virtual ~TProcess_Reader_Create_Impl() = default;

      TProcess_Reader_Create_Impl& operator = (TProcess_Reader_Create_Impl const&) = delete;
      TProcess_Reader_Create_Impl& operator = (TProcess_Reader_Create_Impl&&) noexcept = delete;

      [[nodiscard]] virtual std::expected<std::pair<bool, std::string>, MyErrorInfo> HasDbStructureForBerlin() const override;
      [[nodiscard]] virtual std::expected<bool, MyErrorInfo> DropDbStructureForBerlin() override;
      [[nodiscard]] virtual std::expected<bool, MyErrorInfo> CreateDbStructureForBerlin() override;

   private:
      static const TProcess_Reader_Impl::entity_type_set structure_info_berlin_old;
      static const std::vector<std::string>              create_statements_berlin_old;
   };
