#pragma once

#include <string>

class TMyForm;

/**
 * \brief Interface für die eigentlichen Geschäftsvorfälle, die der Klasse TProcess mit Hilfe der Teilprozesse verarbeitet werden
*/
class TBusinessOperations {
   private:
	   std::string strApplication;  /// Bezeichnung der Anwendung, für welche die Geschäftsprozesse bereitgestellt werden
		int         iMajorVersion;   /// Hauptversionsnummer für die Anwendung / Geschäftsprozesse
		int         iMinorVersion;   /// Unterversionsnummer für die Anwendung / Geschäftsprozesse
	public:
		TBusinessOperations(void);
		TBusinessOperations(TBusinessOperations const& ref);
		TBusinessOperations(TBusinessOperations&& ref) noexcept;
		virtual ~TBusinessOperations(void);

		void swap(TBusinessOperations& ref) noexcept;
		void copy(TBusinessOperations const& ref);

		virtual std::string ApplicationText(void);

		virtual void Init(TMyForm&&) = 0;
		virtual void Login(void) = 0;
		virtual void ImportBerlin(void) = 0;
   };
