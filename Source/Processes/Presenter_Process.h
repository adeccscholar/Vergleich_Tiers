#pragma once

#include "adecc_Scholar/MyForm.h"

/**
  \brief Klasse mit den Teilprozessen mit den aktiven Teilprozessen
*/
class TProcess_Presenter {
   public:
      TProcess_Presenter() = default;
      TProcess_Presenter(TProcess_Presenter const&) = default;
      TProcess_Presenter(TProcess_Presenter&& ref) noexcept { swap(ref); }

     virtual ~TProcess_Presenter() = default;

      TProcess_Presenter& operator =(TProcess_Presenter const&) = default;
      TProcess_Presenter& operator =(TProcess_Presenter&&) noexcept = default;

      void swap(TProcess_Presenter &ref) noexcept { }
      void copy(TProcess_Presenter const& ref) { }

};
