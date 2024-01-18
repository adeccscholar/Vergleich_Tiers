#pragma once

#include <type_traits>
#include <string_view>
#include <format>

namespace MySafety {

template <typename ty>
concept my_integral_ty = std::is_integral_v<std::remove_cvref_t<ty>> &&
                         !std::is_same_v<std::remove_cvref_t<ty>, bool>;

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
      strict,
      sign_error,
      interval_error,
      convert_error,
      unknown,
      stateless = 999
   };

/** Internal enumeration type with the types of checks. This type is used so that runtime decisions
are avoided in a central checking method */
enum class EInternChecks : uint32_t {
     none = 0,
     compare,
     add,
     substract,
     increment,
     decrement,
     unary_minus,
     multiply,
     divide,
     modulo,
     construct,
     assign,
     function,
     convert,
     unknown
   };

template <typename ty>
concept my_number_check = std::is_enum_v<ty> &&
                          std::is_same_v<std::underlying_type_t<ty>, uint32_t>&&
                          std::is_same_v<ty, EInternChecks>;


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
      withPosition       = 256,
      withDivideByZero   = 512
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

template <uint32_t ControlValue>
concept exist_output_for_safety = (ControlValue == 0) ||
                                  (ControlValue != 0 && (IsSafetyFlagSet<ControlValue, ENumberSafety::withException>() ||
                                                         IsSafetyFlagSet<ControlValue, ENumberSafety::withTrace>() ||
                                                         IsSafetyFlagSet<ControlValue, ENumberSafety::withAdditionalData>()));

template <uint32_t ControlValue>
concept has_only_status = IsSafetyFlagSet<ControlValue, ENumberSafety::withAdditionalData>() &&
                          !IsSafetyFlagSet<ControlValue, ENumberSafety::withException>();

template <uint32_t ControlValue>
concept has_status = IsSafetyFlagSet<ControlValue, ENumberSafety::withAdditionalData>();

template <uint32_t ControlValue>
concept has_optional = IsSafetyFlagSet<ControlValue, ENumberSafety::withOptionalChecks>();

template <uint32_t ControlValue>
concept has_interval = IsSafetyFlagSet<ControlValue, ENumberSafety::withIntervalChecks>();

template <uint32_t ControlValue>
concept has_position = IsSafetyFlagSet<ControlValue, ENumberSafety::withPosition>();

template <uint32_t ControlValue>
concept withTrace = IsSafetyFlagSet<ControlValue, ENumberSafety::withTrace>();

template <uint32_t ControlValue>
concept withException = IsSafetyFlagSet<ControlValue, ENumberSafety::withException>();

template <uint32_t ControlValue>
concept withStrictTypes = IsSafetyFlagSet<ControlValue, ENumberSafety::withStrictTypes>();

template <uint32_t ControlValue>
concept withRangeChecks = IsSafetyFlagSet<ControlValue, ENumberSafety::withRangeChecks>();

template <uint32_t ControlValue>
concept withIntervallChecks = IsSafetyFlagSet<ControlValue, ENumberSafety::withIntervalChecks>();

template <uint32_t SAFETY>
concept is_optional_or_has_status = has_optional<SAFETY> || has_status<SAFETY>;

template <uint32_t SAFETY, uint32_t SAFETY_OTHER>
concept are_optional_or_have_status = is_optional_or_has_status<SAFETY> || is_optional_or_has_status<SAFETY_OTHER>;


constexpr uint32_t DefaultSafety = combineNumberSafety(ENumberSafety::withException,
                                                       ENumberSafety::withAdditionalData,
                                                       ENumberSafety::withOptionalChecks,
                                                       //ENumberSafety::withIntervalChecks,
                                                       ENumberSafety::withPosition,
                                                       ENumberSafety::withRangeChecks,
                                                       //ENumberSafety::withStrictTypes,
                                                       ENumberSafety::withOverflowChecks,
                                                       ENumberSafety::withDivideByZero);

constexpr uint32_t StandardSafety = combineNumberSafety(ENumberSafety::withException,
                                                        ENumberSafety::withPosition,
                                                        ENumberSafety::withRangeChecks,
                                                        ENumberSafety::withOverflowChecks,
                                                        ENumberSafety::withDivideByZero);

constexpr uint32_t SilentSafety   = combineNumberSafety(ENumberSafety::withAdditionalData,
                                                        ENumberSafety::withRangeChecks,
                                                        ENumberSafety::withOverflowChecks,
                                                        ENumberSafety::withDivideByZero);


} // close namespace to define std::formatter


template <>
struct std::formatter<MySafety::ENumberStatus> : std::formatter<std::string_view> {
   template <typename FormatContext>
   auto format(MySafety::ENumberStatus t, FormatContext& ctx) const {
      string_view out = "undefined";
      switch (t) {
         case MySafety::ENumberStatus::ok:             out = "ok";                    break;
         case MySafety::ENumberStatus::uninitialized:  out = "uninitialized value";   break;
         case MySafety::ENumberStatus::inexact:        out = "inexact value";         break;
         case MySafety::ENumberStatus::underflow:      out = "underflow error";       break;
         case MySafety::ENumberStatus::overflow:       out = "overflow error";        break;
         case MySafety::ENumberStatus::divbyzero:      out = "divide by zero error";  break;
         case MySafety::ENumberStatus::invalid:        out = "invalid value";         break;
         case MySafety::ENumberStatus::range:          out = "range error";           break;
         case MySafety::ENumberStatus::unexpected:     out = "unexpected value";      break;
         case MySafety::ENumberStatus::strict:         out = "strict types error";    break;
         case MySafety::ENumberStatus::convert_error:  out = "convert error";         break;
         case MySafety::ENumberStatus::interval_error: out = "interval error";        break;
         case MySafety::ENumberStatus::sign_error:     out = "sign error";            break;
         case MySafety::ENumberStatus::unknown:        out = "unknown error";         break;
         case MySafety::ENumberStatus::stateless:      out = "value is stateless";    break;
         default: out = "undefined status, critical";
         }
      return std::formatter<std::string_view>::format(out, ctx);
      }
   };


   template <>
   struct std::formatter<MySafety::EInternChecks> : std::formatter<std::string_view> {
      template <typename FormatContext>
      auto format(MySafety::EInternChecks t, FormatContext& ctx) const {
         string_view out = "undefined";
         switch (t) {
         case MySafety::EInternChecks::none:        out = "none";                  break;
         case MySafety::EInternChecks::compare:     out = "compare";               break;
         case MySafety::EInternChecks::add:         out = "addition";              break;
         case MySafety::EInternChecks::substract:   out = "substraction";          break;
         case MySafety::EInternChecks::increment:   out = "increment";             break;
         case MySafety::EInternChecks::decrement:   out = "decrement";             break;
         case MySafety::EInternChecks::unary_minus: out = "unary minus";           break;
         case MySafety::EInternChecks::multiply:    out = "multiplication";        break;
         case MySafety::EInternChecks::divide:      out = "division";              break;
         case MySafety::EInternChecks::modulo:      out = "modulo division";       break;
         case MySafety::EInternChecks::assign:      out = "assignment operator";   break;
         case MySafety::EInternChecks::construct:   out = "constructing";          break;
         case MySafety::EInternChecks::function:    out = "function call";         break;
         case MySafety::EInternChecks::convert:     out = "convert function";      break;
         case MySafety::EInternChecks::unknown:     out = "unknown";               break;
         default: out = "undefined check method, critical";
         }
         return std::formatter<std::string_view>::format(out, ctx);
      }
   };

   std::ostream& operator << (std::ostream& out, MySafety::EInternChecks const& val) {
      return out << std::format("check for {}", val);
      }

   std::ostream& operator << (std::ostream& out, MySafety::ENumberStatus const& val) {
      return out << std::format("status [{}]", val);
      }
