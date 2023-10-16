#pragma once

#if __has_include("MyExceptions.h")
#include "MyExceptions.h"
#include "MyTrace.h"
#elif __has_include("adecc_Tools/MyExceptions.h")
#include "adecc_Tools/MyExceptions.h"
#include "adecc_Tools/MyTrace.h"
#endif

#include <string_view>
#include <format>
#include <source_location>
#include <chrono>

template <typename ty>
concept my_integral_ty = std::is_integral_v<std::remove_cvref_t<ty>> &&
                         !std::is_same_v<std::remove_cvref_t<ty>, bool>;

/*
template <typename ty>
concept my_integral_ty = std::is_same_v<std::remove_cvref_t<ty>, int>;
*/
template <typename ty>
concept my_number_ty = my_integral_ty<ty>;

template <my_number_ty ty>
constexpr bool two_s_complement() {
   if constexpr (!std::is_signed_v<ty>) return false;
   else if constexpr (std::numeric_limits<ty>::min() < 0 - std::numeric_limits<ty>::max()) return true;
   else return false;
   }

template<typename ty>
inline constexpr bool always_false_safe_number = false;


enum class ENumberStatus : uint32_t {
      ok  = 0,
      uninitialized,
      inexact, 
      underflow,
      overflow, 
      divbyzero, 
      invalid, 
      range, 
      unexpected,
      unknown
   };


template <>
struct std::formatter<ENumberStatus> : std::formatter<std::string_view> {
   template <typename FormatContext>
   auto format(ENumberStatus t, FormatContext& ctx) {
      string_view out = "undefined";
      switch (t) {
         case ENumberStatus::ok:            out = "ok";            break;
         case ENumberStatus::uninitialized: out = "uninitialized"; break;
         case ENumberStatus::inexact:       out = "inexact";       break;
         case ENumberStatus::underflow:     out = "underflow";     break;
         case ENumberStatus::overflow:      out = "overflow";      break;
         case ENumberStatus::divbyzero:     out = "divbyzero";     break;
         case ENumberStatus::invalid:       out = "invalid";       break;
         case ENumberStatus::range:         out = "range";         break;
         case ENumberStatus::unexpected:    out = "unexpected";    break;
         case ENumberStatus::unknown:       out = "unknown";       break;
         default: out = "undefined";
         }
      return formatter<string_view>::format(out, ctx);
      }
   };



/** Internal enumeration type with the types of checks. This type is used so that runtime decisions 
are avoided in a central checking method */
enum class EInternChecks : uint32_t {
      none = 0,
      add,
      substract,
      increment,
      decrement,
      unary_minus,
      multiply,
      divide,
      modulo,
      assign,
      function, 
      unknown
   };

enum class ENumberSafety : uint32_t {
      without            =   0,
      withOptionalChecks =   1,
      withTrace          =   2,
      withException      =   4,
      withAdditionalData =   8,
      withIntervalChecks =  16,
      withRangeChecks    =  32,
      withOverflowChecks =  64,
      withStrictTypes    = 128,
      withDivideByZero   = 256
   };

template <typename ty>
concept my_number_safety = std::is_enum_v<ty> && 
                           std::is_same_v<std::underlying_type_t<ty>, uint32_t> &&
                           std::is_same_v<ty, ENumberSafety>;

constexpr uint32_t setNumberSafety(my_number_safety auto first) {
   return static_cast<uint32_t>(first);
   }

template<my_number_safety... Args>
constexpr uint32_t combineNumberSafety(ENumberSafety first, Args... rest) {
   return static_cast<uint32_t>(first) | (... | static_cast<uint32_t>(rest));
   }

template <uint32_t ControlValue, ENumberSafety Flag>
constexpr bool IsSafetyFlagSet() {
   return (ControlValue & static_cast<uint32_t>(Flag)) != 0;
   }


constexpr uint32_t DefaultSafety = combineNumberSafety(ENumberSafety::withException,
                                                       //ENumberSafety::withOptionalChecks,
                                                       //ENumberSafety::withAdditionalData,
                                                       //ENumberSafety::withIntervalChecks,
                                                       ENumberSafety::withRangeChecks,
                                                       ENumberSafety::withOverflowChecks,
                                                       ENumberSafety::withDivideByZero);


template <my_number_ty ty> 
struct NumberBaseOnlyValue {
   ty value = ty { };
   };

struct NumberBaseOptional {
   bool is_initialized;
   };

struct NumberBaseStatus  {
   ENumberStatus status = ENumberStatus::ok;
   };

template <my_number_ty ty>
struct NumberBaseInterval {
   ty minimum = ty{ };
   ty maximum = ty{ };
   bool value_into(ty const& val) { return val >= minimum && val <= maximum;  }
   };


template <my_number_ty ty = int, uint32_t SAFETY = DefaultSafety>
class MySafeNumber final : 
             private NumberBaseOnlyValue<ty>,
             private std::conditional_t<IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>(), NumberBaseOptional, void>,
             private std::conditional_t<IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>(), NumberBaseInterval<ty>, void>,
             private std::conditional_t<IsSafetyFlagSet<SAFETY, ENumberSafety::withAdditionalData>(), NumberBaseStatus, void> {
   friend std::ostream& operator << (std::ostream & out, MySafeNumber const& val) { return out << val.Value(); };
   public:
      using class_type = MySafeNumber<ty>;
      using value_type = ty;
      
      constexpr MySafeNumber() { reset();  }
      constexpr MySafeNumber(MySafeNumber const& other) { copy(other);  }
      constexpr MySafeNumber(MySafeNumber&& other) noexcept { swap(other);  }
      constexpr MySafeNumber(ty const& value) { init(value); }
      ~MySafeNumber() = default;

      // --------------------------------------------------------------------------------
      // assignment operators for the class MySafeNumber
      // --------------------------------------------------------------------------------
      MySafeNumber& operator = (MySafeNumber const& other) {
         copy(other);
         return *this;
         }

      MySafeNumber& operator = (MySafeNumber&& other) noexcept {
         swap(other);
         return *this;
         }

      MySafeNumber& operator = (ty value) {
         init(value);
         return *this;
         }

      // --------------------------------------------------------------------------------
      // conversion operators for the class MySafeNumber
      // --------------------------------------------------------------------------------
      explicit operator ty& ()& { return Value();  }
      explicit operator ty const& () const& { return Value(); }

      template <typename = std::enable_if_t<IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()>>
      operator bool() const { return this->is_initialized; }

   private:
      ty& Number() { return this->value;  }
   public:
      ty const& Value() const { return this->value;  }
      
      template <typename = std::enable_if_t<IsSafetyFlagSet<SAFETY, ENumberSafety::withAdditionalData>()>>
      ENumberStatus const& Status() const { return this->status; }
      
      template <typename = std::enable_if_t<IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>()>>
      ty const& Minimum() const { return this->Minimum; }

      template <typename = std::enable_if_t<IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>()>>
      ty const& Maximum() const { return this->Maximum; }

      template <typename = std::enable_if_t<IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>()>>
      bool Interval(ty const& min, ty const& max) {
         if(min >= max) {
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withException>())
               throw TMy_StandardError<std::range_error>("value for min isn't lesser then max");
            return false;
            }
         for(auto const& val : { min, max }) {
            if(val < std::numeric_limits<ty>::min() && val > std::numeric_limits<ty>::max()) {
               if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withException>())
                  throw TMy_StandardError<std::range_error>("value for min isn't lesser then max");
               return false;
               }
            }
         return true; 
         }


      void Value(ty const& newVal) { this->value = newVal; }
      template <typename = std::enable_if_t<IsSafetyFlagSet<SAFETY, ENumberSafety::withAdditionalData>()>>
      void Status(ENumberStatus const& newVal) { this->status = newVal;  }

      // -------------------------------------------------------------------------------------------
      // comparsion operators for the class MySafeNumber
      // -------------------------------------------------------------------------------------------
      auto operator <=> (MySafeNumber const& other) const noexcept {
         return Value() <=> other.Value();
         }

      auto operator == (MySafeNumber const& other) const noexcept { return operator <=> (other) == 0; }
      auto operator != (MySafeNumber const& other) const noexcept { return operator <=> (other) != 0; }
      auto operator <  (MySafeNumber const& other) const noexcept { return operator <=> (other) <  0; }
      auto operator <= (MySafeNumber const& other) const noexcept { return operator <=> (other) <= 0; }
      auto operator >  (MySafeNumber const& other) const noexcept { return operator <=> (other) >  0; }
      auto operator >= (MySafeNumber const& other) const noexcept { return operator <=> (other) >= 0; }

      MySafeNumber& operator ++() {
         if constexpr (SAFETY > 0) Safe<EInternChecks::increment>();
         else ++Number();
         return *this;
         }

      MySafeNumber operator ++(int) {
         MySafeNumber result(*this);
         if constexpr (SAFETY > 0) {
            Safe<EInternChecks::increment>();
            }
         else ++Number();
         return result; // eventuell Status übertragen, Design entscheidung
         }

      MySafeNumber& operator --() {
         if constexpr (SAFETY > 0) Safe<EInternChecks::decrement>();
         else --Number();
         return *this;
         }

      MySafeNumber operator --(int) {
         MySafeNumber result(*this);
         if constexpr (SAFETY > 0) {
            Safe<EInternChecks::decrement>();
            }
         else --Number();
         return result; // eventuell Status übertragen, Design entscheidung
      }

      // 2er Komplement prüfen
      MySafeNumber operator - () const  {
         if constexpr (SAFETY > 0) {
            MySafeNumber result(*this);
            result.Safe<EInternChecks::unary_minus>();
            return result;
            }
         else return MySafeNumber(-Value());
         }


      MySafeNumber& operator += (MySafeNumber const& other) {
         if constexpr (SAFETY > 0) Safe<EInternChecks::add>(other);
         else Number() += other.Value();
         return *this;
         }

      MySafeNumber& operator += (int value) {
         if constexpr (SAFETY > 0) {
            MySafeNumber other(value);
            Safe<EInternChecks::add>(other);
            }
         else Number() += value;
         return *this;
         }

      [[nodiscard]] MySafeNumber operator + (MySafeNumber const& other) const {
         MySafeNumber result(*this);
         if constexpr (SAFETY > 0) {
            result.Safe<EInternChecks::add>(other);
            }
         else result.Number() += other.Value();
         return result;
         }

      [[nodiscard]] MySafeNumber operator + (ty value) const {
         MySafeNumber result(*this);
         if constexpr (SAFETY > 0) {
            MySafeNumber other(value);
            result.Safe<EInternChecks::add>(other);
            }
         else result.Number() += value;
         return result;
         }

      friend [[nodiscard]] MySafeNumber operator + (ty value, MySafeNumber const& other) {
         MySafeNumber result(value);
         if constexpr (SAFETY > 0) {
            result.Safe<EInternChecks::add>(other);
            }
         else result.Number() += other.Value();
         return result;
         }

      MySafeNumber& operator -= (MySafeNumber const& other) {
         if constexpr (SAFETY > 0) Safe<EInternChecks::substract>(other);
         else Number() -= other.Value();
         return *this;
         }

      MySafeNumber& operator -= (ty value) {
         if constexpr (SAFETY > 0) {
            MySafeNumber other(value);
            Safe<EInternChecks::substract>(other);
            }
         else Number() -= value;
         return *this;
         }

      [[nodiscard]] MySafeNumber operator - (MySafeNumber const& other) const {
         MySafeNumber result(*this);
         if constexpr (SAFETY > 0) result.Safe<EInternChecks::substract>(other); 
         else result.Number() += other.Value();
         return result;
         }

      [[nodiscard]] MySafeNumber operator - (ty value) const {
         MySafeNumber result(*this);
         if constexpr (SAFETY > 0) result.Safe<EInternChecks::substract>(MySafeNumber(value) );
         else result.Number() -= value;
         return result;
         }

      friend [[nodiscard]] MySafeNumber operator - (ty value, MySafeNumber const& other) {
         MySafeNumber result(value);
         if constexpr (SAFETY > 0) {
            result.Safe<EInternChecks::substract>(other);
            }
         else result.Number() -= other.Value();
         return result;
         }

      MySafeNumber& operator *= (MySafeNumber const& other) {
         if constexpr (SAFETY > 0) Safe<EInternChecks::multiply>(other);
         else Number() *= other.Value();
         return *this;
         }

      MySafeNumber& operator *= (ty value) {
         if constexpr (SAFETY > 0) {
            MySafeNumber other(value);
            Safe<EInternChecks::multiply>(other);
            }
         else Number() *= value;
         return *this;
         }

      [[nodiscard]] MySafeNumber operator * (MySafeNumber const& other) const {
         MySafeNumber result(*this);
         if constexpr (SAFETY > 0) {
            result.Safe<EInternChecks::multiply>(other);
            }
         else result.Number() *= other.Value();
         return result;
         }

      [[nodiscard]] MySafeNumber operator * (ty value) const {
         MySafeNumber result(*this);
         if constexpr (SAFETY > 0) {
            MySafeNumber other(value);
            result.Safe<EInternChecks::multiply>(other);
            }
         else result.Number() *= value;
         return result;
         }

      friend [[nodiscard]] MySafeNumber operator * (ty value, MySafeNumber const& other) {
         MySafeNumber result(value);
         if constexpr (SAFETY > 0) {
            result.Safe<EInternChecks::multiply>(other);
            }
         else result.Number() *= other.Value();
         return result;
         }

      MySafeNumber& operator /= (MySafeNumber const& other) {
         if constexpr (SAFETY > 0) Safe<EInternChecks::divide>(other);
         else Number() /= other.Value();
         return *this;
         }

      MySafeNumber& operator /= (ty value) {
         if constexpr (SAFETY > 0) {
            MySafeNumber other(value);
            Safe<EInternChecks::divide>(other);
            }
         else Number() /= value;
         return *this;
         }

      [[nodiscard]] MySafeNumber operator / (MySafeNumber const& other) const {
         MySafeNumber result(*this);
         if constexpr (SAFETY > 0) {
            result.Safe<EInternChecks::divide>(other);
            }
         else result.Number() /= other.Value();
         return result;
         }

      [[nodiscard]] MySafeNumber operator / (ty value) const {
         MySafeNumber result(*this);
         if constexpr (SAFETY > 0) {
            MySafeNumber other(value);
            result.Safe<EInternChecks::divide>(other);
            }
         else result.Number() /= value;
         return result;
         }

      friend [[nodiscard]] MySafeNumber operator / (ty value, MySafeNumber const& other) {
         MySafeNumber result(value);
         if constexpr (SAFETY > 0) {
            result.Safe<EInternChecks::divide>(other);
            }
         else result.Number() /= other.Value();
         return result;
         }

      MySafeNumber& operator %= (MySafeNumber const& other) {
         if constexpr (SAFETY > 0) Safe<EInternChecks::modulo>(other);
         else Number() %= other.Value();
         return *this;
         }

      MySafeNumber& operator %= (ty value) {
         if constexpr (SAFETY > 0) {
            MySafeNumber other(value);
            Safe<EInternChecks::modulo>(other);
            }
         else Number() %= value;
         return *this;
         }

      [[nodiscard]] MySafeNumber operator % (MySafeNumber const& other) const {
         MySafeNumber result(*this);
         if constexpr (SAFETY > 0) {
            result.Safe<EInternChecks::modulo>(other);
            }
         else result.Number() %= other.Value();
         return result;
         }

      [[nodiscard]] MySafeNumber operator % (ty value) const {
         MySafeNumber result(*this);
         if constexpr (SAFETY > 0) {
            MySafeNumber other(value);
            result.Safe<EInternChecks::modulo>(other);
            }
         else result.Number() %= value;
         return result;
         }

      friend [[nodiscard]] MySafeNumber operator % (ty value, MySafeNumber const& other) {
         MySafeNumber result(value);
         if constexpr (SAFETY > 0) {
            result.Safe<EInternChecks::modulo>(other);
            }
         else result.Number() %= other.Value();
         return result;
         }


      [[nodiscard]] bool is_even(void) const {
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()) {
            if (!CheckOpional(EInternChecks::function, std::source_location::current(), std::chrono::system_clock::now())) return false;
            }
         return Number() % 2 == 0;
         } 

      [[nodiscard]] bool is_odd(void) const {
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()) {
            if (!CheckOpional(EInternChecks::function, std::source_location::current(), std::chrono::system_clock::now())) return false;
            }
         return !is_even();
         }

      [[nodiscard]] bool is_negative(void) const {
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()) {
            if (!CheckOpional(EInternChecks::function, std::source_location::current(), std::chrono::system_clock::now())) return false;
            }
         return Value() < 0;
         }

      [[nodiscard]] MySafeNumber abs(void) const {
         if (is_negative()) return -(*this);
         else  return *this; 
         }

      [[nodiscard]] MySafeNumber pow(MySafeNumber const& exponent) {
         if (exponent.is_negative()) throw std::range_error("pow with negative exponent");
         if (Value() == 0) [[unlikely]] return MySafeNumber(0);
         else if (exponent == MySafeNumber(0)) return MySafeNumber(1);
         else [[likely]] {
            MySafeNumber result(*this);
            for (MySafeNumber i = 1; i < exponent; i++) {
               result *= (*this);
               }
            return result;
            }
         }

      template <typename = std::enable_if_t<IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>()>>
      bool is_value_into(void) const { return this->value_into(Value());  }

      // ------------------------------------------------------------
      constexpr void reset(void) {
         Value(ty { } );
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>())
            this->is_initialized = false;
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withAdditionalData>())
            this->status = ENumberStatus::ok;
         if constexpr (SAFETY != 0 && !IsSafetyFlagSet<SAFETY, ENumberSafety::withException>() &&
                                      !IsSafetyFlagSet<SAFETY, ENumberSafety::withTrace>() &&
                                      !IsSafetyFlagSet<SAFETY, ENumberSafety::withAdditionalData>()) {
            static_assert(always_false_safe_number<ty>, "unhandled variable (SafeNumber)");
            }
         }

      constexpr void init(ty newVal) {
         Value(newVal);
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>())
            this->is_initialized = true;
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withAdditionalData>())
            this->status = ENumberStatus::ok;
         if constexpr (SAFETY != 0 && !IsSafetyFlagSet<SAFETY, ENumberSafety::withException>() &&
                                      !IsSafetyFlagSet<SAFETY, ENumberSafety::withTrace>() &&
                                      !IsSafetyFlagSet<SAFETY, ENumberSafety::withAdditionalData>()) {
            static_assert(always_false_safe_number<ty>, "unhandled variable (SafeNumber)");
            }
         }

      constexpr void swap(MySafeNumber& other) noexcept {
         std::swap(this->value, other.value);
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>())
            std::swap(this->is_initialized, other.is_initialized);
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withAdditionalData>())
            std::swap(this->status, other.status);
         }

      constexpr void copy(MySafeNumber const& other) {
         Value(other.Value());
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>())
            this->is_initialized = other.is_initialized;
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withAdditionalData>())
            this->status = other.status;
         }

      // method for unary operations
      /*
      enum class EInternChecks : uint32_t {
      add,         substract,    multiply,     divide,    modulo,
      increment,   decrement,    unary_minus,
      assign,
      */
      template <typename exception_ty> 
      constexpr void Message(EInternChecks kind, ENumberStatus stat, std::string const& strMessage,
                             std::source_location const& loc, 
                             std::chrono::time_point<std::chrono::system_clock> const& now) {
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withAdditionalData>()) {
            Status(stat);
            }
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withTrace>()) {
            Trace<true>(strMessage, std::cerr, loc, now);
            Trace<true>("called at: ", std::cerr);
            }
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withException>()) {
            throw TMy_StandardError<exception_ty>(strMessage, loc, now);
            }
         }

      constexpr bool CheckOpional(EInternChecks kind, std::source_location const& loc,
                                  std::chrono::time_point<std::chrono::system_clock> const& now) {
         if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()) {
            if (!this->is_initialized) {
               Message<std::runtime_error>(kind, ENumberStatus::uninitialized, "MySafeNumber isn't initialized", loc, now);
               return false;
               }
            }
         return true;
         }

      template <EInternChecks CheckKind = EInternChecks::none>
      constexpr void Safe(std::source_location loc = std::source_location::current(),
                           std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now()) {
         //----------------------------------------------------------------------------------------
         if constexpr (CheckKind == EInternChecks::increment) {
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()) {
               if (!CheckOpional(CheckKind, loc, now)) return;
               }
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>()) {
               if (Value() < Maximum()) ++Number();
               else Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
               }
            else if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOverflowChecks>()) {
               if (Value() < std::numeric_limits<ty>::max()) ++Number();
               else Message<std::overflow_error>(CheckKind, ENumberStatus::overflow, "overflow error detected in MySafeNumber", loc, now);
               }
            else ++Number();
            }
         //----------------------------------------------------------------------------------------
         else if constexpr (CheckKind == EInternChecks::decrement) {
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()) {
               if (!CheckOpional(CheckKind, loc, now)) return;
            }
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>()) {
               if (Value() > Minimum()) --Number();
               else Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
            }
            else if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOverflowChecks>()) {
               if (Value() > std::numeric_limits<ty>::min()) --Number();
               else Message<std::underflow_error>(CheckKind, ENumberStatus::underflow, "underflow error detected in MySafeNumber", loc, now);
            }
            else --Number();
            }
         //----------------------------------------------------------------------------------------
         else if constexpr (CheckKind == EInternChecks::unary_minus) {
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()) {
               if (!CheckOpional(CheckKind, loc, now)) return;
               }
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>()) {
               if(!this->is_value_into(-Value())) {
                  Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
                  }
               }
            else if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOverflowChecks>()) {
               if constexpr (two_s_complement<ty>()) {
                  if (Value() != std::numeric_limits<ty>::min()) Number() = -Value();
                  else Message<std::range_error>(CheckKind, ENumberStatus::overflow, "range error in unary -", loc, now);
                  }
               else Number() = -Value();
               }
            }
         //----------------------------------------------------------------------------------------

         }

      template <EInternChecks CheckKind = EInternChecks::none>
      constexpr void Safe(MySafeNumber const& other,
                          std::source_location loc = std::source_location::current(),
                          std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now()) {
         //---------------------------------------------------------------------------------------
         if constexpr (CheckKind == EInternChecks::add) {
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()) {
               if (!CheckOpional(CheckKind, loc, now) || !other.CheckOpional(CheckKind, loc, now)) return;
               }
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>()) {
               if (other.Value() >= 0) [[likely]] {
                  if (Maximum() - other.Value() >= Value()) [[likely]] Number() += other.Value();
                  else Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
                  }
               else {
                  if (Minimum() - other.Value() <= Value()) [[likely]] Number() += other.Value();
                  else Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
                  }
               }
            else if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOverflowChecks>()) {
               if (other.Value() >= 0) [[likely]] {
                  if (std::numeric_limits<ty>::max() - other.Value() >= Value()) [[likely]] Number() += other.Value();
                  else Message<std::overflow_error>(CheckKind, ENumberStatus::overflow, "overflow error detected in MySafeNumber", loc, now);
                  }
               else {
                  if (std::numeric_limits<ty>::min() - other.Value() <= Value()) [[likely]] Number() += other.Value();
                  else Message<std::underflow_error>(CheckKind, ENumberStatus::underflow, "underflow error detected in MySafeNumber", loc, now);
                  }
               }
            else Number() += other.Value();
            }
         //---------------------------------------------------------------------------------------
         else if constexpr (CheckKind == EInternChecks::substract) {
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()) {
               if (!CheckOpional(CheckKind, loc, now) || !other.CheckOpional(CheckKind, loc, now)) return;
               }
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>()) {
               if (other.Number() >= 0) {
                  if (Minimum() - other.Value() <= Value()) [[likely]] Number() -= other.Value();
                  else Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
                  }
               else {
                  if (Maximum() - other.Value() >= Value()) [[likely]] Number() -= other.Value();
                  else Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
                  }
               }
            else if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOverflowChecks>()) {
               if (other.Number() >= 0) {
                  if (std::numeric_limits<ty>::min() - other.Value() <= Value()) [[likely]] Number() -= other.Value();
                  else Message<std::overflow_error>(CheckKind, ENumberStatus::overflow, "overflow error detected in MySafeNumber", loc, now);
                  }
               else {
                  if (std::numeric_limits<ty>::max() - other.Value() >= Value()) [[likely]] Number() -= other.Value();
                  else Message<std::underflow_error>(CheckKind, ENumberStatus::underflow, "underflow error detected in MySafeNumber", loc, now);
                  }
               }
            else Number() -= other.Value();
            }
         //---------------------------------------------------------------------------------------
         else if constexpr (CheckKind == EInternChecks::multiply) {
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()) {
               if (!CheckOpional(CheckKind, loc, now) || !other.CheckOpional(CheckKind, loc, now)) return;
               }
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>()) {
               if (Value() > 0) {
                  if (other.Value() > 0) {
                     if (Value() < Maximum() / other.Value()) [[likely]] Number() *= other.Value();
                     else Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
                     }
                  else if (other.Value() < 0) {
                     if (Value() < Minimum() / other.Value()) [[likely]] Number() *= other.Value();
                     else Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
                     }
                  else [[unlikely]] Number() *= other.Value();
                  }
               else if (Value() < 0) {
                  if (other.Value() < 0) {
                     if (Value() < Maximum() / other.Value()) [[likely]] Number() *= other.Value();
                     else Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
                     }
                  else if (other.Value() > 0) {
                     if (Value() > Minimum() / other.Value()) [[likely]] Number() *= other.Value();
                     else Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
                     }
                  else Number() *= other.Value();
                  }
               else [[unlikely]]Number() *= other.Value();
               }
            else if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOverflowChecks>()) {
               if (Value() > 0) {
                  if (other.Value() > 0) {
                     if(Value() < std::numeric_limits<ty>::max() / other.Value()) [[likely]] Number() *= other.Value();
                     else Message<std::overflow_error>(CheckKind, ENumberStatus::overflow, "overflow error detected in MySafeNumber", loc, now);
                     }
                  else if (other.Value() < 0) {
                     if(Value() < std::numeric_limits<ty>::min() / other.Value()) [[likely]] Number() *= other.Value();
                     else Message<std::underflow_error>(CheckKind, ENumberStatus::underflow, "underflow error detected in MySafeNumber", loc, now);
                     }
                  else [[unlikely]] Number() *= other.Value();
                  }
               else if (Value() < 0) {
                  if (other.Value() < 0) {
                     if(Value() < std::numeric_limits<ty>::max() / other.Value()) [[likely]] Number() *= other.Value();
                     else Message<std::overflow_error>(CheckKind, ENumberStatus::overflow, "overflow error detected in MySafeNumber", loc, now);
                     }
                  else if (other.Value() > 0) {
                     if(Value() > std::numeric_limits<ty>::min() / other.Value()) [[likely]] Number() *= other.Value();
                     else Message<std::underflow_error>(CheckKind, ENumberStatus::underflow, "underflow error detected in MySafeNumber", loc, now);
                     }
                  else [[unlikely]] Number() *= other.Value();
                  }
               else [[unlikely]] Number() *= other.Value();
               }
            else Number() *= other.Value();               
            }
         //---------------------------------------------------------------------------------------

         else if constexpr (CheckKind == EInternChecks::divide) {
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()) {
               if (!CheckOpional(CheckKind, loc, now) || !other.CheckOpional(CheckKind, loc, now)) return;
               }
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withDivideByZero>()) {
               if (other.Value() != 0) [[likely]] {
                  if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>()) {
                     ty result = Number() / other.Value();
                     if(this->is_value_into(result)) Number() = result;
                     else Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
                     }
                  else Number() /= other.Value();
                  }
               else Message<std::runtime_error>(CheckKind, ENumberStatus::divbyzero, "division by zero in MySafeNumber", loc, now);
               }
            else if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>()) {
               ty result = Number() / other.Value();
               if (this->is_value_into(result)) Number() = result;
               else Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
               }
            else Number() /= other.Value();
            }
         //---------------------------------------------------------------------------------------
         else if constexpr (CheckKind == EInternChecks::modulo) {
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()) {
               if (!CheckOpional(CheckKind, loc, now) || !other.CheckOpional(CheckKind, loc, now)) return;
               }
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withDivideByZero>()) {
               if (other.Value() != 0) [[likely]] {
                  if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>()) {
                     ty result = Number() % other.Value();
                     if (this->is_value_into(result)) Number() = result;
                     else Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
                  }
                  else Number() %= other.Value();
                  }
               else Message<std::runtime_error>(CheckKind, ENumberStatus::divbyzero, "division by zero in MySafeNumber", loc, now);
               }
            else if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withIntervalChecks>()) {
               ty result = Number() % other.Value();
               if (this->is_value_into(result)) Number() = result;
               else Message<std::range_error>(CheckKind, ENumberStatus::range, "interval error detected in MySafeNumber", loc, now);
               }
            else Number() %= other.Value();
            }
         //---------------------------------------------------------------------------------------
         }
         
   };

   template <my_number_ty ty = int>
   using SafeWithInterval = MySafeNumber<ty, combineNumberSafety(ENumberSafety::withException
                                                                 , ENumberSafety::withOptionalChecks
                                                                 //, ENumberSafety::withAdditionalData
                                                                 , ENumberSafety::withIntervalChecks
                                                                 //, ENumberSafety::withRangeChecks
                                                                 //, ENumberSafety::withOverflowChecks
                                                                 , ENumberSafety::withDivideByZero
                                                                 )>;
