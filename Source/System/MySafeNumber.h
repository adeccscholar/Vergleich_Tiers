#pragma once

#if __has_include("MyTrace.h")
#include "MyTrace.h"
#elif __has_include("adecc_Tools/MyTrace.h")
#include "adecc_Tools/MyTrace.h"
#endif

#if __has_include("Intern/MySafeNumberDefines.h")
#include "Intern/MySafeNumberDefines.h"
#include "Intern/MySafeNumberBase.h"
#elif __has_include("System/Intern/MySafeNumberDefines.h")
#include "System/Intern/MySafeNumberDefines.h" 
#include "System/Intern/MySafeNumberBase.h"
#endif

#include <string>
#include <typeinfo>
#include <format>
#include <source_location>
#include <type_traits>

namespace MySafety {

   template <typename ty>
   concept is_my_safe_number_type = requires {
      typename ty::value_type;
      typename ty::class_type;
      { std::declval<typename ty::value_type>() } -> my_number_ty;
      { std::declval<const ty>().get_value() } -> std::same_as<std::expected<typename ty::value_type, TValueError<typename ty::value_type>>>;
      std::is_default_constructible_v<ty>;
      std::copy_constructible<ty>;
      std::move_constructible<ty>;
   };

   template <typename ty>
   concept is_my_safe_number_optional = is_my_safe_number_type<ty> && std::is_base_of_v<NumberBaseOptional, ty>&&
      requires(ty t) {
         { t.is_initialized } -> std::same_as<bool>;
      };

   template <my_number_ty ty = int, uint32_t SAFETY = DefaultSafety>
   class TNumber final : private NumberBaseOnlyValue<ty>,
                         private std::conditional_t<has_optional<SAFETY>, NumberBaseOptional,                void>,
                         private std::conditional_t<has_interval<SAFETY>, NumberBaseInterval<ty>,            void>,
                         private std::conditional_t<has_status<SAFETY>,   NumberBaseStatus,                  void>,
                         private std::conditional_t<has_position<SAFETY>, NumberBasePosition<ty, TRACE_LEN>, void> {
      friend std::ostream& operator << (std::ostream & out, TNumber const& val) { 
         if constexpr (has_optional<SAFETY>) if(!val.is_initialized) return out << "<empty>"; 
         if constexpr (has_status<SAFETY>) if (val.status != ENumberStatus::ok) return out << std::format("{} {}", val.value, val.status);         
         return out << val.value;
         }

      public:
         using class_type       = TNumber<ty, SAFETY>;
         using class_type_param = std::conditional_t<has_position<SAFETY>, TValue<TNumber<ty, SAFETY>, false>, TNumber<ty, SAFETY>>;
         using value_type       = ty;
         using value_type_param = std::conditional_t<has_position<SAFETY>, TValue<value_type, false>, value_type>;
      
         template <typename = std::enable_if_t<!has_position<SAFETY>>>
         constexpr TNumber() { 
            if constexpr (withTrace<SAFETY>) Trace("standard constructor called");
            _reset(); 
            }

         template <typename = std::enable_if_t<has_position<SAFETY>>>
         constexpr TNumber(src_loc const& loc = src_loc::current(), time_stamp const& now = std::chrono::system_clock::now()) { 
            if constexpr (withTrace<SAFETY>) Trace(std::format("standard constructor called from {}", MyPostionTimeStamp(loc, now)));
            if constexpr (has_position<SAFETY>) AddPosition("standard constructor called", loc, now);
            _reset(); 
            }


         constexpr TNumber(TNumber<ty, SAFETY> const& other) { copy(other); }
         constexpr TNumber(TNumber<ty, SAFETY>&& other) noexcept { swap(other); }

         template <my_number_ty other_ty> requires has_position<SAFETY>
         TNumber(other_ty const& value,  
                          src_loc const& loc = src_loc::current(), time_stamp const& now = std::chrono::system_clock::now()) {
            if constexpr (has_position<SAFETY>) {
               AddPosition("constructor called", loc, now);
               AddPosition("constructor with value of my_number_ty and position- informations", 
                           src_loc::current(), std::chrono::system_clock::now());
               }
            safe_init<other_ty>(value, EInternChecks::construct);
            }

         template <my_number_ty other_ty> requires !has_position<SAFETY>
         TNumber(other_ty const& value) {
            if constexpr (has_position<SAFETY>) {
               AddPosition("constructor with val of type my_number_ty", src_loc::current(), std::chrono::system_clock::now());
               }
            safe_init<other_ty>(value, EInternChecks::construct);
            }

         template <my_number_ty other_ty>
         TNumber(other_ty const& value, bool boInitialized, ENumberStatus eStatus) : TNumber() {
            if constexpr (has_position<SAFETY>) {
               AddPosition("constructor with val of type my_number_ty and addition", src_loc::current(), std::chrono::system_clock::now());
               }
            if(safe_init<other_ty>(value)) {
               if constexpr (has_optional<SAFETY>) this->is_initialized = boInitialized;
               if constexpr (has_status<SAFETY>)   this->status = eStatus;
               }
            }

         // param_number_or_value_type_as_number  other_ty_param_ty
         template <param_has_number_value_type other_ty>
         TNumber(other_ty const& value) {
            if constexpr (has_position<SAFETY>) {
               if constexpr (param_has_position<other_ty>) AddPosition("constructor called", value.loc(), value.now());
               AddPosition("constructor with param has value_type of my_number", src_loc::current(), std::chrono::system_clock::now());
               }
            safe_init<other_ty>(value);
            }

          ~TNumber() = default;

          // --------------------------------------------------------------------------------
          // assignment operators for the class TNumber
          // --------------------------------------------------------------------------------
          TNumber& operator = (TNumber const& other) {
             copy(other);
             return *this;
             }

          TNumber& operator = (TNumber&& other) noexcept {
             swap(other);
             return *this;
             }

         // template <param_number_or_value_type_as_number other_ty>
          template <other_ty_param_ty other_ty>
          TNumber& operator = (other_ty const& value) {
             if constexpr (has_position<SAFETY>) {
                if constexpr (param_has_position<other_ty>)AddPosition("assignment operator with value_type called", value.loc(), value.now());
                AddPosition("assignment operator");
                }
             safe_init<other_ty>(value, EInternChecks::assign);
             return *this;
             }
         
         // -------------------------------------------------------------------------------------------------
         // relationals operators
         


         // -------------------------------------------------------------------------------------------------
         // methods for the value
         //ty const& Value(void) const { return this->value; }

         constexpr std::expected<value_type, TValueError<value_type>> get_value() const { 
            if constexpr (has_optional<SAFETY>) {
               if (!this->is_initialized) return std::unexpected(TValueError{ this->value, true, ENumberStatus::uninitialized });
               }
            if constexpr (has_status<SAFETY>) {
               if (this->status != ENumberStatus::ok) return std::unexpected(TValueError{ this->value, false, this->status });
               }
            return this->value; 
            }


         template <my_number_ty other_ty> requires has_position<SAFETY>
         void Value(other_ty const& newVal,
                    src_loc const& loc = src_loc::current(), time_stamp const& now = std::chrono::system_clock::now()) {
            if constexpr (withTrace<SAFETY>) {
               CallTrace(std::format("Value({}) called from", newVal), loc, now);
               CallTrace(std::format("Value({})", newVal));
               }
            if constexpr (has_position<SAFETY>) {
               AddPosition("set value function called", loc, now);
               AddPosition("set value function");
               }
            safe_init<other_ty>(EInternChecks::convert, newVal);
            }

         template <my_number_ty other_ty> requires !has_position<SAFETY>
         void Value(other_ty const& newVal) { 
            if constexpr (withTrace<SAFETY>) CallTrace(std::format("Value({})", newVal));
            safe_init<other_ty>(EInternChecks::convert, newVal);
            }

         // ------------------------------------------------------------------------------------------------
         // methods for the position

         template <typename = std::enable_if_t<has_interval<SAFETY>>>
         NumberBaseInterval<ty> const& Interval() const { return static_cast<NumberBaseInterval<ty> const&>(*this); };

         template <typename = std::enable_if_t<has_interval<SAFETY>>>
         NumberBaseInterval<ty> & Interval() { return static_cast<NumberBaseInterval<ty>&>(*this); };

         template <typename = std::enable_if_t<has_interval<SAFETY>>>
         bool value_in_interval(ty const& val) { return Interval().value_into(val); }

         template <typename = std::enable_if_t<has_interval<SAFETY>>>
         bool set_interval(ty const& min, ty const& max) { 
            return Interval().set_interval_value(min, max); 
            }
         

         // ------------------------------------------------------------------------------------------------
         // methods for the position

         template <typename = std::enable_if_t<has_position<SAFETY>>>
         myPositions<ty, TRACE_LEN> const& Positions() const { return this->positions(); };

         template <typename = std::enable_if_t<has_position<SAFETY>>>
         void AddPosition(std::string const& msg, src_loc loc = src_loc::current(), time_stamp now = std::chrono::system_clock::now()) const {
            std::optional<bool> opt_val;
            std::optional<ENumberStatus> opt_stat;
            if constexpr (has_optional<SAFETY>) opt_val = this->is_initialized;
            else opt_val = std::nullopt;
            if constexpr (has_status<SAFETY>) opt_stat = this->status;
            else opt_stat = std::nullopt;
            this->locations.push(std::make_tuple(msg, this->value, opt_val, opt_stat, loc, now));
            }

         template <typename = std::enable_if_t<has_position<SAFETY>>>
         void ClearPosition() {
            this->positions().clear();
            }

         template <typename = std::enable_if_t<has_position<SAFETY>>>
         void WritePosition(std::ostream& out) const {
            for (uint32_t i = 0; auto & val : this->positions().view()) {
               out << std::format("{:2d}: {}\n", ++i, val);
               }
            }

         // ------------------------------------------------------------------------------------------------
         // methods for additional status
         template <typename = std::enable_if_t<has_status<SAFETY>>>
         void Status(ENumberStatus const& newVal) { this->status = newVal; }


         //template <typename = std::enable_if_t<has_status<SAFETY>>>
         ENumberStatus Status(void) const {
            if constexpr (has_status<SAFETY>) return this->status;
            else return ENumberStatus::stateless;
            }

         // ------------------------------------------------------------------------------------------------
         // methods for additional optional informations
         template <typename = std::enable_if_t<has_optional<SAFETY>>>
         bool Is_Initialized() const { return this->is_initialized; }

         // =============================================================================================
         /// method to reset the variable
         [[nodiscard]] constexpr bool is_negative(void) const {
            if constexpr (has_optional<SAFETY>) {
               if (!CheckOpional(EInternChecks::function)) return false;
               }
            return Value() < 0;
            }

         // =============================================================================================
         /// method to reset the variable

         constexpr void _reset(void) {
            if constexpr (!exist_output_for_safety<SAFETY>) {
               static_assert(always_false_safe_number<ty>, "unhandled variable (SafeNumber)");
               }
            this->value = ty{ };
            if constexpr (has_optional<SAFETY>) this->is_initialized = false;
            if constexpr (has_status<SAFETY>)   Status(ENumberStatus::uninitialized);
            }

         template <typename = std::enable_if_t<!has_position<SAFETY>>>
         constexpr void reset(void) {
            _reset();
            }

         template <typename = std::enable_if_t<has_position<SAFETY>>>
         constexpr void reset(src_loc const& loc = src_loc::current(), time_stamp const& now = std::chrono::system_clock::now()) {
            _reset();
            if constexpr (has_position<SAFETY>) {
               ClearPosition();
               AddPosition("reset called", loc, now);
               }
            }

         constexpr void swap(TNumber& other) noexcept {
            std::swap(this->value, other.value);
            if constexpr (has_interval<SAFETY>)  static_cast<NumberBaseInterval&>(*this).swap(static_cast<NumberBaseInterval&>(other));
            if constexpr (has_optional<SAFETY>)  std::swap(this->is_initialized, other.is_initialized);
            if constexpr (has_status<SAFETY>)    std::swap(this->status,         other.status);
            if constexpr (has_position<SAFETY>)  this->positions().swap(other.positions);
            }

         constexpr void copy(TNumber const& other) {
            Value(other.Value());
            if constexpr (has_interval<SAFETY>)  static_cast<NumberBaseInterval&>(*this).copy(static_cast<NumberBaseInterval&>(other));
            if constexpr (has_optional<SAFETY>)  this->is_initialized = other.is_initialized;
            if constexpr (has_status<SAFETY>)    this->status = other.status;
            if constexpr (has_position<SAFETY>)  this->positions().copy(other.positions());
            }


         // =============================================================================================
         template <typename = std::enable_if_t<withTrace<SAFETY>>>
         void CallTrace(std::string const& strMsg, src_loc const& loc = src_loc::current(), 
                    time_stamp const& now = std::chrono::system_clock::now()) {
            ENumberStatus status;
            if constexpr (has_status<SAFETY>) status = this->status;
            else status = ENumberStatus::stateless;
            std::string strOpt;
            if constexpr (has_optional<SAFETY>) strOpt = this->is_initialized ? "(initialized)"s : "uninitialized"s;
            else strOpt = "(mandatory)"s;
            Trace<true>(std::format("{2:}, Value<{3:}>  {1:} - Status: {0:}", status, strOpt, strMsg, typeid(ty).name()), std::cerr, loc, now);
            }

         // --------------------------------------------------------------------------------------------------------------
         template <typename exception_ty>
         constexpr void Message(EInternChecks kind, std::string const& strMessage,
                                src_loc const& loc = src_loc::current(), 
                                time_stamp const& now = std::chrono::system_clock::now()) const {
            if constexpr (withException<SAFETY>) {
               std::ostringstream os;
               ENumberStatus status;         // local value for this property, self when its not exist in this instance
               bool          is_initialized; // local value for this property, self when its not exist in this instance
               if constexpr (has_status<SAFETY>) status = this->status;
               else status = ENumberStatus::stateless;
               if constexpr (has_optional<SAFETY>) is_initialized = this->is_initialized;
               else is_initialized = true;
               
               if constexpr (has_position<SAFETY>)
                  throw TMyNumberError<value_type, exception_ty>(strMessage, this->value, SAFETY, kind, status, is_initialized, this->locations, loc, now);
               else
                  throw TMyNumberError<value_type, exception_ty>(strMessage, this->value, SAFETY, kind, status, is_initialized, {}, loc, now);
               }
            }

         // ---------------------------------------------------------------------------------------------
         template <typename = std::enable_if_t<!has_position<SAFETY>>>
         constexpr bool CheckOpional(EInternChecks kind) const {
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()) {
               if (!this->is_initialized) {
                  if constexpr (has_status<SAFETY>) this->status = ENumberStatus::uninitialized;
                  Message<std::runtime_error>(kind, "requested Number isn't initialized");
                  return false;
                  }
               }
            return true;
            }


         template <typename = std::enable_if_t<has_position<SAFETY>>>
         constexpr bool CheckOpional(EInternChecks kind, src_loc const& loc, time_stamp const& now) const {
            if constexpr (IsSafetyFlagSet<SAFETY, ENumberSafety::withOptionalChecks>()) {
               if (!this->is_initialized) {
                  if constexpr (has_status<SAFETY>) this->status = ENumberStatus::uninitialized;
                  AddPosition("Checkoptional called from here", loc, now);
                  AddPosition("Message from CheckOptional called", src_loc::current(), std::chrono::system_clock::now());
                  Message<std::runtime_error>(kind, ENumberStatus::uninitialized, "Number isn't initialized");
                  return false;
                  }
               }
            return true;
            }

         // ---------------------------------------------------------------------------------------------
         template <ENumberStatus kind, my_integral_ty other_ty> requires my_integral_ty<ty>
         bool can_init(EInternChecks check_kind, other_ty const& value_other, NumberBaseInterval<biggest_int_of<other_ty, ty>> const& interval) {
            if constexpr (std::is_signed_v<other_ty> && std::is_unsigned_v<ty>) {
               if (value_other < 0) {
                  if constexpr (has_position<SAFETY>) AddPosition("can't init integral, sign check"s);
                  this->value = ty{};
                  if constexpr (has_status<SAFETY>)   this->status = ENumberStatus::sign_error;
                  if constexpr (has_optional<SAFETY>) this->is_initialized = false;
                  std::ostringstream os;
                  os << "error in " << check_kind << ", sign error, negative value " << value_other
                     << "for the unsigned type \"" << typeid(ty).name() << "\".";
                  Message<std::runtime_error>(check_kind, os.str());
                  return false;
                  }
               }           
            if (interval.value_into(static_cast<biggest_int_of<other_ty, ty>>(value_other))) [[likely]] {
               this->value = static_cast<ty>(value_other);
               if constexpr (has_status<SAFETY>)   this->status = ENumberStatus::ok;
               if constexpr (has_optional<SAFETY>) this->is_initialized = true;
               return true;
               }
            else {
               if constexpr (has_position<SAFETY>) AddPosition("can't init integral, out of range. message method call"s);
               this->value = ty{};
               if constexpr (has_status<SAFETY>)   this->status = kind;
               if constexpr (has_optional<SAFETY>) this->is_initialized = false;
               std::ostringstream os;
               os << "error in " << check_kind << " to underlaying datatype \"" << typeid(ty).name() << "\", value = " << value_other;

               if constexpr (kind == ENumberStatus::range) os << " not in limits " << interval << ".";
               else os << "not in interval " << interval << ".";

               Message<std::runtime_error>(check_kind, os.str());
               return false;
               }
            }

         template <param_number_or_value_type_as_number other_ty>
         auto get_value_for_number_or_safe_number(EInternChecks check_kind, other_ty const& newVal) -> typename value_type_select<other_ty>::type const& {
            if constexpr (param_has_number_value_type<other_ty>) {
               if (auto val = newVal.get_value(); val.has_value()) return val.value();
               else {
                  if constexpr (has_optional<SAFETY>) this->is_initialized = !val.error().boEmpty;
                  if constexpr (has_status<SAFETY>) this->status = val.error().status;
                  if constexpr (has_position<SAFETY>) AddPosition(std::format("get value with {}{}{}",
                           val.error().boEmpty ? "with empty value" : "",
                           val.error().boEmpty && val.error().status != ENumberStatus::ok ? " and " : "",
                           val.error().status != ENumberStatus::ok ? std::format("Status {}", val.error().status) : ""));
                  Message<std::runtime_error>(check_kind, "requested value of paramter for operation is undefined");
                  return val.error().value;
                  }
               }
            else return newVal;
            };




         template <param_number_or_value_type_as_number other_ty>
         auto safe_compare(other_ty const& compVal) {
            if constexpr (!exist_output_for_safety<SAFETY>) {
               static_assert(always_false_safe_number<ty>, "unhandled variable (SafeNumber)");
               }

            TNumber<ty, SAFETY> otherVal;
            safe_init<other_ty>(compVal, EInternChecks::compare);



            return;
            }

         template <param_number_or_value_type_as_number other_ty>
         bool safe_init(other_ty const& newVal, EInternChecks check_kind) {
            if constexpr (!exist_output_for_safety<SAFETY>) {
               static_assert(always_false_safe_number<ty>, "unhandled variable (SafeNumber)");
               }

            
            using value_type = typename value_type_select<other_ty>::type;
            
            if constexpr (has_position<SAFETY>) AddPosition("init function");
       
            if constexpr (!std::is_same_v<ty, value_type> && withStrictTypes<SAFETY>) {
               if constexpr (has_status<SAFETY>) this->status = ENumberStatus::strict;
               if constexpr (has_optional<SAFETY>) this->is_initialized = false;
               if constexpr (has_position<SAFETY>) AddPosition("init function detect sign error, message method call at");
               std::ostringstream os;
               os << "strict datatypes error in " << check_kind << " to underlaying datatype \"" << typeid(ty).name() 
                  << "\", type of paramter is \"" << typeid(value_type).name() << "\" , value = " << newVal << ".";
               Message<std::runtime_error>(check_kind, os.str());
               return false;
               }
            else { 
               auto value_other = get_value_for_number_or_safe_number(check_kind, newVal);
               if constexpr (std::is_same_v<ty, value_type>) {
                  this->value = value_other;
                  if constexpr (has_status<SAFETY>) this->status = ENumberStatus::ok;
                  if constexpr (has_optional<SAFETY>) this->is_initialized = true;
                  return true;
                  }
               else {
                  if constexpr (std::is_integral_v<value_type> && std::is_integral_v<ty>) { 
                     if constexpr (withIntervallChecks<SAFETY> || withRangeChecks<SAFETY>) { 
                        biggest_int_of<ty, value_type> min, max;
                        if constexpr (withIntervallChecks<SAFETY>) {
                           if constexpr (std::is_unsigned_v<value_type> && std::is_signed<ty>) {
                              min = (this->minimum() < 0) ? ty(0) : this->minimum();
                              max = (this->maximum() < 0) ? ty(0) : this->maximum();
                              if(max <= min) {
                                 if constexpr (has_position<SAFETY>) AddPosition("init function detect interval error, message method call at");
                                 Message<std::runtime_error>(check_kind,
                                            std::format("interval error through unsigned \"{}\" and signed {}.\nInterval {}",
                                            typeid(value_type).name(), typeid(ty).name(), Interval()));
                                 return false;
                                 }
                              }
                           else {
                              min = this->minimum();
                              max = this->maximum();
                              }
                           return can_init<ENumberStatus::interval_error>(check_kind, value_other, { min, max } );
                           }
                        else if constexpr (withRangeChecks<SAFETY>) {
                           min = std::is_unsigned_v<value_type> ? ty (0) : std::numeric_limits<ty>::min();
                           max = std::numeric_limits<ty>::max();
                           return can_init<ENumberStatus::range>(check_kind, value_other, { min, max });
                           } 
                        }
                     else {
                        this->value = static_cast<ty>(value_other);
                        if constexpr (has_status<SAFETY>)   this->status = ENumberStatus::ok;
                        if constexpr (has_optional<SAFETY>) this->is_initialized = true;
                        return true;
                        }
                     }
                  }
               }
            }
      };


                                                          

}  // end of namespace MySafety


