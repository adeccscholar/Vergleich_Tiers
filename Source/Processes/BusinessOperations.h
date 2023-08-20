#pragma once

#include <string>

/**
 * \brief Interface für die eigentlichen Geschäftsvorfälle, die der Klasse TProcess mit Hilfe der Teilprozesse verarbeitet werden
*/
class TBusinessOperations {
   protected:
	   std::string strApplication;
	public:
		TBusinessOperations(void) { strApplication = "GeoDatenApp 2.0"s; }
		TBusinessOperations(TBusinessOperations const& ref) { copy(ref);  }
		TBusinessOperations(TBusinessOperations&& ref) noexcept { swap(ref); }
		virtual ~TBusinessOperations(void) { };

		void swap(TBusinessOperations& ref) noexcept { std::swap(strApplication, ref.strApplication);  }
		void copy(TBusinessOperations const& ref) { strApplication = ref.strApplication; }

		virtual void Login(void) = 0;
   };
