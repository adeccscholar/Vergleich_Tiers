#pragma once


#if __has_include("MyExceptions.h")
  #include "MyExceptions.h"
#elif __has_include("adecc_Tools/MyExceptions.h")
  #include "adecc_Tools/MyExceptions.h"
#endif

#if __has_include("MySafeNumberDefines.h")
  #include "MySafeNumberDefines.h"
  #include "MyCircularBuffer.h"
#elif __has_include("Intern/MySafeNumberDefines.h")
  #include "Intern/MySafeNumberDefines.h"
  #include "Intern/MyCircularBuffer.h"
#elif __has_include("System/Intern/MySafeNumberDefines.h")
  #include "System/Intern/MySafeNumberDefines.h" 
  #include "System/Intern/MyCircularBuffer.h"
#endif

#include <tuple>
#include <utility>
#include <source_location>
#include <chrono>
#include <limits>
#include <typeinfo>
#include <expected>

namespace MySafety {

   const size_t TRACE_LEN = 10;

   template <typename ty>
   using myPosition = std::tuple<std::string, ty, std::optional<bool>, std::optional<ENumberStatus>, src_loc, time_stamp>;

   template <typename ty, size_t SIZE = TRACE_LEN>
   using myPositions = CircularBuffer<myPosition<ty>, SIZE>;

   template <typename ty>
   struct TValueError {
      ty   value;
      bool boEmpty;
      ENumberStatus status;
      };

   template <typename ty, bool WITHPOS>
   class TValue : private std::conditional_t<WITHPOS == true, myPositions<ty>, void> {
      ty value;
      src_loc    current_location;
      time_stamp current_time_stamp;

      public:
         using value_type = ty;
         constexpr TValue() : TValue(ty { })  { }
         constexpr TValue(ty const& val, 
                          src_loc const& p_loc = src_loc::current(), 
                          time_stamp const& p_now = std::chrono::system_clock::now())  {
               value = val;
               current_location   = p_loc;
               current_time_stamp = p_now;
               if constexpr (WITHPOS == true) Positions().push({ "value initialized", value, std::nullopt, std::nullopt, current_location, current_time_stamp });
               }

         constexpr TValue(TValue const& other) { copy(other); }
         constexpr TValue(TValue&&) noexcept = default;
         ~TValue() = default;

         TValue& operator = (TValue const& other) { copy(other); return *this; }

         constexpr operator value_type& ()& { return value; }
         constexpr operator value_type const& () const& { return value; }

         //std::expected<value_type&, TValueError> get_value() { return value; }
         constexpr std::expected<value_type, TValueError<value_type>> get_value() const { return value;  }

         template <typename = std::enable_if_t<WITHPOS == true>>
         constexpr myPositions<ty>& Positions() { return static_cast<myPositions<ty>&>(*this); }

         template <typename = std::enable_if_t<WITHPOS == true>>
         constexpr myPositions<ty> const& Positions() const { return static_cast<myPositions<ty> const&>(*this); }

         constexpr src_loc    const& loc() const { return current_location; }
         constexpr time_stamp const& now() const { return current_time_stamp; }

         void copy(TValue const& other) {
            value = other.value;
            current_location = other.current_location;
            current_time_stamp = other.current_time_stamp;
            if constexpr (WITHPOS == true) static_cast<myPositions<ty>&>(*this).copy(static_cast<myPositions<ty>&>(other));
            }

         void swap(TValue&& other) noexcept {
            std::swap(value, other.value);
            std::swap(current_location, other.current_location);
            std::swap(current_time_stamp, other.current_time_stamp);
            if constexpr (WITHPOS == true) static_cast<myPositions<ty>&>(*this).swap(static_cast<myPositions<ty>&>(other));
            }

         };

   template <typename ty>
   TValue(ty const&) -> TValue<ty, false>;

   template <typename other_ty, uint32_t SAFETY>
   using other_ty_param = std::conditional_t<has_position<SAFETY>, TValue<other_ty, false>, other_ty>;

   template <typename ty>
   concept param_has_value_type = requires {
      typename ty::value_type;
      };

   template <typename ty>
   concept param_has_number_value_type = requires {
      typename ty::value_type; 
      { std::declval<typename ty::value_type>() } -> my_number_ty;
      { std::declval<const ty>().get_value() } -> std::same_as<std::expected<typename ty::value_type, TValueError<typename ty::value_type>>>;
      };

   template <typename ty>
   concept param_has_position = requires(const ty t) {
      typename ty::value_type;
      { t.loc() } -> std::same_as<const src_loc&>;
      { t.now() } -> std::same_as<const time_stamp&>;
      };

   template <typename ty>
   concept param_number_or_value_type_as_number = param_has_number_value_type<ty> || my_number_ty<ty>;

   template <typename ty> 
   concept param_check_or_value_type_as_check = my_number_check<ty> 
                                                || (param_has_value_type<ty> && my_number_check<typename ty::value_type>);


   template <typename ty, typename = void>
   struct value_type_select {
      using type = ty;
      };

   template <typename ty>
   struct value_type_select<ty, std::enable_if_t<param_has_number_value_type<ty>>> {
      using type = typename ty::value_type;
      };


   template <typename ty>
   concept other_ty_param_ty = my_number_ty<ty> || (param_has_number_value_type<ty> && my_number_ty<typename ty::value_type>);

   template<my_integral_ty ty1, my_integral_ty ty2>
   using biggest_int_of = std::conditional_t<(sizeof(ty1) > sizeof(ty2)) ||
                                              (sizeof(ty1) == sizeof(ty2) && std::is_unsigned_v<ty1>), ty1, ty2>;

   template <my_number_ty ty>
   struct NumberBaseOnlyValue {
      ty value = ty{ };
      };

   struct NumberBaseOptional {
      bool is_initialized;
      };


   template <my_number_ty ty, size_t SIZE = 10>
   class NumberBasePosition {
      friend void swap(NumberBasePosition& lhs, NumberBasePosition& rhs) noexcept { lhs.swap(rhs); }
      protected:
         mutable myPositions<ty, SIZE> locations;
      public:
         NumberBasePosition() = default;
         NumberBasePosition(NumberBasePosition const& other) { copy(other); }
         NumberBasePosition(NumberBasePosition && other) noexcept { swap(other); }
         ~NumberBasePosition() = default;

         myPositions<ty, SIZE> const& positions() const { return locations; }
         myPositions<ty, SIZE> &      positions()  { return locations; }

         void copy(NumberBasePosition const& other) { locations.copy(other.locations);  }
         void swap(NumberBasePosition & other) noexcept { locations.swap(other.locations); }
      };

   struct NumberBaseStatus {
      mutable ENumberStatus status = ENumberStatus::ok;
      };

   template <my_number_ty ty>
   class NumberBaseInterval {
      friend void swap(NumberBaseInterval& lhs, NumberBaseInterval& rhs) noexcept { lhs.swap(rhs); }
      friend std::ostream& operator << (std::ostream& out, NumberBaseInterval const& val) {
         return out << "( " << val.interval.first << ", " << val.interval.second << ")";
         }

      private:
         std::pair<ty, ty> interval;
      public:
         NumberBaseInterval(ty const& min = std::numeric_limits<ty>::min(), ty const& max = std::numeric_limits<ty>::max()) {
            interval.first  = min;
            interval.second = max;
            }

         NumberBaseInterval(NumberBaseInterval const& other) { copy(other);  }
         NumberBaseInterval(NumberBaseInterval&& other) noexcept { swap(other); }
         ~NumberBaseInterval() = default;

         ty const& minimum() const { return interval.first; }
         void      minimum(ty const& val) { interval.first = val; }
         ty const& maximum() const { return interval.second; }
         void      maximum(ty const& val) { interval.second = val; }

         bool value_into(ty const& val) const { return val >= minimum() && val <= maximum(); }

         bool set_interval_value(ty const& min, ty const& max) {
            if (min >= max) return false;
            else {
               interval.first  = min;
               interval.second = max;
               return true;
               }
            }

         void copy(NumberBaseInterval const& other) {
            interval = other.interval;
            }

         void swap(NumberBaseInterval& other) noexcept {
            swap(interval, other.interval);
            }
      };

   class TNumberError_Base {
      public:
         virtual std::string NumberStatus() const = 0;
         virtual std::string PositionsText() const = 0;
         virtual std::string ShortMessage() const = 0;
         virtual std::string FullMessage() const = 0;
      };

   
class TNumberExceptionInformation : public TMyExceptionInformation {
private:
   uint32_t       iSafety;
   EInternChecks eCheck_kind;
   ENumberStatus eStatus;
   bool          boInitialized;
public:
   TNumberExceptionInformation(uint32_t pSafety, EInternChecks pCheck_kind, ENumberStatus pStatus, bool pInitialized,
                        src_loc const& loc = src_loc::current(), time_stamp const& timept = std::chrono::system_clock::now())
      : TMyExceptionInformation(loc, timept), iSafety(pSafety), eCheck_kind(pCheck_kind), eStatus(pStatus), boInitialized(pInitialized) { }

   TNumberExceptionInformation(TNumberExceptionInformation const& other) { copy(other); }

   virtual ~TNumberExceptionInformation() = default;

   uint32_t      Safety() const { return iSafety; }
   EInternChecks Check_kind() const { return eCheck_kind; }
   ENumberStatus Status() const { return eStatus; }
   bool          Initialized() const { return boInitialized; }

   virtual std::string NumberStatus() const {
      return std::format("Error detected during {}\nStatus = {}, value {}",
              Check_kind(), Status(), (Initialized() ? "is initialized" : "isn't initialized"));
      }


   void copy(TNumberExceptionInformation const& other) {
      static_cast<TMyExceptionInformation&>(*this).copy(static_cast<TMyExceptionInformation const&>(other));
      iSafety        = other.iSafety;
      eCheck_kind    = other.eCheck_kind;
      eStatus        = other.eStatus;
      boInitialized  = other.boInitialized;
      }
};

template <MyWrappedException exception_ty>
class TMyNumberError : public exception_ty, public TNumberExceptionInformation {
private:
   using used_exception_type = exception_ty;
   using base_type           = TNumberExceptionInformation;
   mutable std::string strMessage; ///< Hilfsvariable für die Rückgabe von what
public:
   TMyNumberError(std::string const& strMsg, uint32_t pSafety, EInternChecks pCheck_kind, ENumberStatus pStatus, bool pInitialized,
      src_loc const& loc = src_loc::current(), time_stamp const& timept = std::chrono::system_clock::now())
      : exception_ty(strMsg), TNumberExceptionInformation(pSafety, pCheck_kind, pStatus, pInitialized, loc, timept) { }

   TMyNumberError(TMyNumberError const& other) : exception_ty(other), TNumberExceptionInformation(other) { }
   virtual ~TMyNumberError() = default;

   const char* what() const noexcept override {
      std::ostringstream os;
      os << "exception of type [" << typeid(used_exception_type).name()
         << "] detected, triggered within MySaftey::TNumber." << std::endl
         << typename used_exception_type::what();
      strMessage = os.str();
      return strMessage.c_str();
      }

};

}  // end of namespace MySafety


template <typename ty>
struct std::formatter<MySafety::NumberBaseInterval<ty>> : std::formatter<std::string_view> {
   template <typename FormatContext>
   auto format(MySafety::NumberBaseInterval<ty> t, FormatContext& ctx) {
      auto it = ctx.out();
      std::format_to(it, "({}, {})", t.minimum(), t.maximum());
      return it;
      }
   };


template <typename ty>
struct std::formatter<MySafety::myPosition<ty>> : std::formatter<std::string_view> {
   template <typename FormatContext>
   auto format(MySafety::myPosition<ty> t, FormatContext& ctx) {
      auto it = ctx.out();
      auto const cz_ts = std::chrono::current_zone()->to_local(std::get<5>(t));
      auto const millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::get<5>(t).time_since_epoch()) % 1000;
      std::string status = std::get<3>(t).has_value() ? std::format("{}", std::get<3>(t).value()) : "stateless"s;
      std::format_to(it, "[{:%d.%m.%Y %X},{:03d}] {}: Value[{}] = ", cz_ts, millis.count(), std::get<0>(t), typeid(ty).name());

      if(std::get<2>(t).has_value())
         if(std::get<2>(t).value() == true)
            std::format_to(it, "{} obligatory [{}]", std::get<1>(t), status);
         else
            std::format_to(it, "<empty> obligatory [{}]", status);
      else
         std::format_to(it, "{} mandatory [{}]", std::get<1>(t), status);


      std::format_to(it, " in function \"{}\" within file \"{}\" in line {}]", std::get<4>(t).function_name(),
                           TMyExceptionInformation::cutPath(std::get<4>(t).file_name()), std::get<4>(t).line());
      return it;
      }
   };


