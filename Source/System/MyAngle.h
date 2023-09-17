#pragma once


#include <iostream>
#include <cmath>
#include <string>
#include <numbers>
#include <type_traits>
#include <format>

template <typename ty>
concept my_integral_param = std::is_integral_v<ty> && !std::is_same_v<ty, bool>;

template <typename ty>
concept my_angle_param = std::is_floating_point_v<ty> || my_integral_param<ty>;

/** @todo checks für Manipulatoren ergänzen
*         Kostruktor für MyAngle<ty, ?> mit MySexagesimalAngle<ty>
*         Konvertierung Operaterar für MyAngle<ty, ?> zu MySexagesimalAngle<ty>
*/



enum class MyAngleKind : int { degree, radian };

template<typename T>
inline constexpr bool always_false_angle = false;

template <typename ty>
concept my_param_angle = std::is_floating_point_v<ty> || (std::is_integral_v<ty> && !std::is_same_v<ty, bool>);

template <typename ty, MyAngleKind kind>
concept my_param_angle_Valid = (kind == MyAngleKind::degree) || std::is_floating_point_v<ty>;

template <my_param_angle ty = double, MyAngleKind kind = MyAngleKind::degree>
   requires my_param_angle_Valid<ty, kind>
class MyAngle {
   friend std::ostream& operator << (std::ostream& out, MyAngle<ty, kind> const& angle) {
      return out << static_cast<ty>(angle) << " " << angle.unit();
      }

   template <MyAngleKind oth_knd, my_param_angle oth_ty = ty>
      requires my_param_angle_Valid<oth_ty, oth_knd>
   friend MyAngle<ty, kind> operator + (MyAngle<ty, kind> const& lhs, MyAngle<oth_ty, oth_knd> const& rhs) {
      return MyAngle<ty, kind> { lhs } += rhs.convert_to<kind, ty>();
      }

   template <MyAngleKind oth_knd, my_param_angle oth_ty = ty>
      requires my_param_angle_Valid<oth_ty, oth_knd>
   friend MyAngle<ty, kind> operator - (MyAngle<ty, kind> const& lhs, MyAngle<oth_ty, oth_knd> const& rhs) {
      return MyAngle<ty, kind> { lhs } -= rhs.convert_to<kind, ty>();
      }

   template <my_angle_param oth_ty>
      requires my_param_angle_Valid<oth_ty, oth_knd>
   friend MyAngle<ty, kind> operator * (MyAngle<ty, kind> const& lhs, oth_ty const& rhs) {
      return MyAngle<ty, kind> { lhs } *= rhs;
      }

   template <my_angle_param oth_ty>
      requires my_param_angle_Valid<oth_ty, oth_knd>
   friend MyAngle<ty, kind> operator / (MyAngle<ty, kind> const& lhs, oth_ty const& rhs) {
      return MyAngle<ty, kind> { lhs } /= rhs;
      }

   public:
      MyAngle(void) = default;

      template <typename oth_ty>
      MyAngle(oth_ty const& val, std::enable_if_t<std::is_floating_point_v<oth_ty> || my_integral_param<oth_ty>, bool>* = nullptr) {
         if constexpr (std::is_same_v<ty, oth_ty>) Angle(val);
         else Angle(static_cast<ty>(val));
         }

      /*
      template <MyAngleKind other_kind, std::floating_point OtherTy>
      MyAngle(MyAngle<OtherTy, other_kind> const& other) {
         if constexpr (other_kind == kind) {
            flAngle = static_cast<ty>(other.flAngle);
            }
         if constexpr (other_kind == MyAngleKind::degree && kind == MyAngleKind::radian) {
            flAngle = static_cast<ty>(other.convert_to<MyAngleKind::radian>());
            }
         else if constexpr (other_kind == MyAngleKind::radian && kind == MyAngleKind::degree) {
            flAngle = static_cast<ty>(other.convert_to<MyAngleKind::degree>());
            }
         else static_assert(always_false_angle<kind>, "invalid kind of angle");
         }
      */

      MyAngle(MyAngle const& other) : flAngle(other.Angle()) { }

      template <MyAngleKind oth_knd, std::floating_point oth_ty = ty, typename = std::enable_if_t<kind == oth_knd && std::is_same_v<ty, oth_ty>>>
      MyAngle(MyAngle<oth_ty, oth_knd>&& other) noexcept {
         std::swap(flAngle, other.flAngle);
         }

      template <MyAngleKind oth_knd, std::floating_point oth_ty = ty>
      MyAngle& operator = (MyAngle<oth_ty, oth_knd> const& other) {
         convert_from<oth_knd>(other);
         return *this;
         }
      
      template <MyAngleKind oth_knd, std::floating_point oth_ty = ty, typename = std::enable_if_t<kind == oth_knd && std::is_same_v<ty, oth_ty>>>
      MyAngle& operator = (MyAngle&& other) noexcept {
         std::swap(flAngle, other.flAngle);
         return *this;
         }

    
      template <MyAngleKind other_kind, std::floating_point OtherTy = ty>
      auto operator <=> (MyAngle<OtherTy, other_kind> const& ref) const {
         static const double epsilon = 1e-6; // Toleranz für Rundungsdifferenzen
         const ty comp = ref.convert_to<kind>().Angle();
         if (std::fabs(flAngle - comp) < epsilon) return std::strong_ordering::equal;
         else if (flAngle < comp) return std::strong_ordering::less;
         else return std::strong_ordering::greater;
         }
     
      
      template <MyAngleKind other_kind, std::floating_point OtherTy = ty>
      bool operator == (MyAngle<OtherTy, other_kind> const& ref)  const { return (*this <=> ref) == 0; }

      template <MyAngleKind other_kind, std::floating_point OtherTy = ty>
      bool operator != (MyAngle<OtherTy, other_kind> const& ref)  const { return (*this <=> ref) != 0; }

      template <MyAngleKind other_kind, std::floating_point OtherTy = ty>
      bool operator <  (MyAngle<OtherTy, other_kind> const& ref)  const { return (*this <=> ref) < 0; }

      template <MyAngleKind other_kind, std::floating_point OtherTy = ty>
      bool operator <= (MyAngle<OtherTy, other_kind> const& ref)  const { return (*this <=> ref) <= 0; }

      template <MyAngleKind other_kind, std::floating_point OtherTy = ty>
      bool operator >  (MyAngle<OtherTy, other_kind> const& ref)  const { return (*this <=> ref) > 0; }

      template <MyAngleKind other_kind, std::floating_point OtherTy = ty>
      bool operator >= (MyAngle<OtherTy, other_kind> const& ref)  const { return (*this <=> ref) >= 0; }

      explicit operator ty& (void) & { return flAngle; }
      explicit operator ty const& (void) const& { return flAngle; }

      operator std::string(void) const { 
         if constexpr (kind == MyAngleKind::degree) return std::format("{} deg", flAngle);
         else if constexpr (kind == MyAngleKind::radian) return std::format("{} rad", flAngle);
         else static_assert(always_false_angle<kind>, "invalid kind of angle for operator std::string");
         }

      template <MyAngleKind oth_knd, my_angle_param oth_ty>
      MyAngle& operator += (MyAngle<oth_ty, oth_knd> const& other) {
         flAngle += create_from<oth_knd>(other).Angle();
         Normalize();
         return *this;
         }

      template <MyAngleKind oth_knd, my_angle_param oth_ty>
      MyAngle& operator -= (MyAngle<oth_ty, oth_knd> const& other) {
         flAngle -= create_from<oth_knd>(other).Angle();
         Normalize();
         return *this;
         }


      template <my_angle_param oth_ty>
      MyAngle& operator *= (oth_ty const& val)  {
         if constexpr (std::is_same_v<ty, oth_ty>) Angle(Angle() * val);
         else Angle(Angle() * static_cast<ty>(val));
         return *this;
         }

      template <my_angle_param oth_ty>
      MyAngle& operator /= (oth_ty const& val) {
         if constexpr (std::is_same_v<ty, oth_ty>) Angle(Angle() / val);
         else Angle(Angle() / static_cast<ty>(val));
         return *this;
         }

      //*   overkill
      template <MyAngleKind oth_knd, std::floating_point oth_ty = ty, typename = std::enable_if_t<kind == oth_knd && std::is_same_v<ty, oth_ty>>>
      operator MyAngle<oth_ty, oth_knd> const& () const {
         return *this;
         }
      
      template <MyAngleKind oth_knd, std::floating_point oth_ty = ty, typename = std::enable_if_t<!std::is_same_v<ty, oth_ty> || kind != oth_knd>>
      operator MyAngle<oth_ty, oth_knd>() const {
         MyAngle<oth_ty, oth_knd> tmp;
         tmp.convert_from(*this);
         return tmp;
         }
      //*/

      /// selector for the datalement with the value of the angle in the unit

      template <MyAngleKind oth_knd = kind>
      constexpr std::conditional_t<oth_knd == kind, ty const&, ty> Angle(void) const {
         if constexpr (oth_knd == kind) return flAngle;
         else if constexpr (oth_knd == MyAngleKind::degree && kind == MyAngleKind::radian) return flAngle * 180.0 / std::numbers::pi_v<ty>; 
         else if constexpr (oth_knd == MyAngleKind::radian && kind == MyAngleKind::degree) return flAngle * std::numbers::pi_v<ty> / 180.0;
         else static_assert(always_false_angle<kind>, "invalid kind of angle");
         }

      constexpr std::string unit(void) const {
         if constexpr (kind == MyAngleKind::degree) return "deg"s;
         else if constexpr (kind == MyAngleKind::radian) return "rad"s;
         else static_assert(always_false_angle<kind>, "invalid kind of angle");
         }

      /* example for a function with exist only for conditions
      template <MyAngleKind other_kind = kind, typename = std::enable_if_t<other_kind == MyAngleKind::degree>>
      ty toRadians(void) const { return flAngle * std::numbers::pi_v<ty> / 180.0; }
      */
      template <typename oth_ty = ty>
      std::conditional_t<(kind == MyAngleKind::radian && std::is_same_v<ty, oth_ty>), oth_ty const&, oth_ty> toRadians(void) const {
         if constexpr (kind == MyAngleKind::radian) {
            if constexpr (std::is_same_v<ty, oth_ty>) return Angle();
            else return static_cast<oth_ty>(Angle());
            }
         else if constexpr (kind == MyAngleKind::degree) {
            if constexpr (std::is_same_v<ty, oth_ty>) return Angle() * std::numbers::pi_v<ty> / 180.0;
            else return static_cast<oth_ty>(Angle() * std::numbers::pi_v<ty> / 180.0);
            }
         else {
            static_assert(always_false_angle<kind>, "invalid kind of angle in toRadian");
            }
         }
      
      
      void Angle(my_angle_param auto const& val) { 
         using oth_ty = decltype(val);
         if constexpr (std::is_same_v<ty, oth_ty>) flAngle = val;
         else {
            if constexpr (std::is_integral_v<ty> && !std::is_integral_v<oth_ty>) flAngle = std::round(val);
            else flAngle = static_cast<ty>(val);
            }
         }

      void Angle(std::string_view val) { auto result = std::from_chars(val.data(), val.data() + val.size(), flAngle); }


      void Normalize() {
         if constexpr (kind == MyAngleKind::degree) {
            static constinit ty max_val = 360.0;
            while (flAngle < 0)        flAngle += max_val; 
            while (flAngle >= max_val) flAngle -= max_val; 
            }
         else if constexpr (kind == MyAngleKind::radian) {
            static constinit ty max_val = 2 * std::numbers::pi_v<ty>;
            while (flAngle < 0.0)      flAngle += max_val;
            while (flAngle >= max_val) flAngle -= max_val; 
            }
         else static_assert(always_false_angle<kind>, "invalid kind of angle in Normalize");
         }


      template <typename oth_ty = ty>
         requires (kind == MyAngleKind::degree)
      bool Check(ty minAngle = 0, ty maxAngle = 360) {
         if (minAngle < 0 || maxAngle > 360) {
            throw std::invalid_argument("A parameter for checking an angle is out of range");
         }
         return (flAngle >= minAngle && flAngle <= maxAngle);
         }


      template <typename oth_ty = ty>
         requires (kind == MyAngleKind::radian) && std::floating_point<ty>
      bool Check(ty minAngle = 0, ty maxAngle = 2 * std::numbers::pi_v<ty>) {
         if (minAngle < 0 || maxAngle > 2 * std::numbers::pi_v<ty>) {
            throw std::invalid_argument("A parameter for checking an angle is out of range");
            }
         return (flAngle >= minAngle && flAngle <= maxAngle);
         }



      template <MyAngleKind other_knd = kind, std::floating_point oth_ty, typename = std::enable_if_t<other_knd == MyAngleKind::degree>>
      void fromRadians(oth_ty const& val) { flAngle = convert<MyAngleKind::radian>(val); }

      template <std::floating_point oth_ty = ty>
      auto sin(void) const {
         if(std::is_same_v<ty, oth_ty>) return std::move(std::sin(toRadians()));
         else return static_cast<oth_ty>(std::sin(toRadians()));
         }

      template <std::floating_point oth_ty = ty>
      auto cos(void) const {
         if (std::is_same_v<ty, oth_ty>) return std::move(std::cos(toRadians()));
         else return static_cast<oth_ty>(std::cos(toRadians()));
         }

      template <std::floating_point oth_ty = ty>
      auto tan(void) const {
         if (std::is_same_v<ty, oth_ty>) return std::move(std::tan(toRadians()));
         else return static_cast<oth_ty>(std::tan(toRadians()));
         }


      /// !!! Baustelle Umkehrfunktionen, nicht von Winkel, Wertebereich, Ergebnis ist Winkel

      template <std::floating_point oth_ty = ty>
      auto asin(void) const {
         if (std::is_same_v<ty, oth_ty>) return std::move(std::asin(toRadians()));
         else return static_cast<oth_ty>(std::asin(toRadians()));
         }

      template <std::floating_point oth_ty = ty>
      auto acos(void) const {
         if (std::is_same_v<ty, oth_ty>) return std::move(std::acos(toRadians()));
         else return static_cast<oth_ty>(std::acos(toRadians()));
         }

      template <std::floating_point oth_ty = ty>
      auto atan(void) const {
         if (std::is_same_v<ty, oth_ty>) return std::move(std::atan(toRadians()));
         else return static_cast<oth_ty>(std::atan(toRadians()));
         }

   private:
      ty flAngle;


   public:
 
      template<MyAngleKind oth_knd, std::floating_point oth_ty = ty>
      MyAngle<ty,kind> create_from(MyAngle<oth_ty, oth_knd> const& other) const {
         MyAngle<ty, kind> ret;
         ret.convert_from(other);
         return ret;
         }

      template<MyAngleKind oth_knd, std::floating_point oth_ty = ty>
      void convert_from(MyAngle<oth_ty, oth_knd> const& other) {
         if constexpr (kind == oth_knd) {
            if constexpr (std::is_same_v<ty, oth_ty>)  Angle(other.Angle());
            else                                       Angle(static_cast<ty>(other.Angle()));
            }
         else if constexpr (kind == MyAngleKind::degree && oth_knd == MyAngleKind::radian) {
            if constexpr (std::is_same_v<ty, oth_ty>)  Angle(other.Angle() * 180.0 / std::numbers::pi_v<ty>);
            else                                       Angle(static_cast<ty>(other.Angle() * 180.0 / std::numbers::pi_v<ty>));
            }
         else if constexpr (kind == MyAngleKind::radian && oth_knd == MyAngleKind::degree) {
            if constexpr (std::is_same_v<ty, oth_ty>)  Angle(other.Angle() * std::numbers::pi_v<ty> / 180.0);
            else                                       Angle(static_cast<ty>(other.Angle() * std::numbers::pi_v<ty> / 180.0));
            }
         else static_assert(always_false_angle<kind>, "invalid kind of angle in convert_from");
         }

      template<MyAngleKind oth_knd, std::floating_point oth_ty = ty>
      std::conditional_t<(kind == oth_knd && std::is_same_v<ty, oth_ty>), MyAngle<oth_ty, oth_knd> const&, MyAngle<oth_ty, oth_knd>> convert_to() const {
         if constexpr (kind == oth_knd) {
            if constexpr (std::is_same_v<ty, oth_ty>) return *this;
            else {
               return MyAngle<oth_ty, oth_knd>(Angle());
               }
            }
         else if constexpr (kind == MyAngleKind::degree && oth_knd == MyAngleKind::radian) {
            return MyAngle<oth_ty, oth_knd>(Angle() * std::numbers::pi_v<ty> / 180.0);
            }
         else if constexpr (kind == MyAngleKind::radian && oth_knd == MyAngleKind::degree) {
            return MyAngle<oth_ty, oth_knd>(Angle() * 180.0 / std::numbers::pi_v<ty>);
            }
         else {
            static_assert(always_false_angle<kind>, "invalid kind of angle in convert_to");
            }
         }

   };


// ----------------------------------------------------------------------------------
/**  namespace to isolate the literals for angles
*/
namespace my_angle_literals {

inline MyAngle<double, MyAngleKind::degree> operator""_deg(long double val) {
   return MyAngle<double, MyAngleKind::degree>(static_cast<double>(val));
   }

inline MyAngle<double, MyAngleKind::radian> operator""_rad(long double val) {
   return MyAngle<double, MyAngleKind::radian>(static_cast<double>(val));
   }

} // end of namespace my_angle_literals


template <my_integral_param ty, my_angle_param sec_ty = ty>
class MySexagesimalAngle : public std::tuple<ty, ty, sec_ty> {

   friend std::ostream& operator << (std::ostream& out, MySexagesimalAngle<ty, sec_ty> const data) {
      out << std::setw(3) << std::right << data.Degrees() << "° "
          << std::setw(2) << std::right << data.Minutes() << "\' ";
      if constexpr (std::is_integral_v<sec_ty>) out << std::setw(2) << std::right << data.Seconds();
      else out << std::setprecision(6) << data.Seconds();
      return out << "\"";
      }

public:
   MySexagesimalAngle(void) = default;
   MySexagesimalAngle(MySexagesimalAngle const&) = default;
   MySexagesimalAngle(MySexagesimalAngle&&) noexcept = default;

   MySexagesimalAngle(ty const& deg, ty const& min, sec_ty const& sec) : std::tuple<ty, ty, sec_ty>(deg, min, sec) { }

   MySexagesimalAngle(std::string const& input) { Parse(input); }

   template <std::floating_point para_ty>
   MySexagesimalAngle(para_ty const& deg_dec) : MySexagesimalAngle<ty>() {
      DegreesAsDecimal(deg_dec);
   }

   ~MySexagesimalAngle(void) = default;

   MySexagesimalAngle& operator = (MySexagesimalAngle const&) = default;
   MySexagesimalAngle& operator = (MySexagesimalAngle&&) noexcept = default;

   template <std::floating_point param_ty = double>
   operator param_ty () const { return DegreesAsDecimal(); }

   template <std::floating_point param_ty = double>
   operator MyAngle<param_ty, MyAngleKind::degree>() const { return MyAngle<param_ty, MyAngleKind::degree>(DegreesAsDecimal()); }

   operator std::string() const { 
      if constexpr (std::is_integral_v<sec_ty>)
         return std::format("{:3d}° {:2d}\' {:3d}\"", Degrees(), Minutes(), Seconds()); 
      else
         return std::format("{:3d}° {:2d}\' {:.6f}\"", Degrees(), Minutes(), Seconds());
      }

   auto operator <=> (MySexagesimalAngle const&) const = default;

   ty     const& Degrees(void) const { return std::get<0>(*this); }
   ty     const& Minutes(void) const { return std::get<1>(*this); }
   sec_ty const& Seconds(void) const { return std::get<2>(*this); }

   template <std::floating_point param_ty = double>
   param_ty DegreesAsDecimal(void) const { return static_cast<param_ty>((Seconds() / 60.0 + Minutes()) / 60.0 + Degrees()); }

   void Degrees(ty const& val) { std::get<0>(*this) = val; }
   void Minutes(ty const& val) { std::get<1>(*this) = val; }
   void Seconds(sec_ty const& val) { std::get<2>(*this) = val; }

   void Degrees(std::string_view val) { auto result = std::from_chars(val.data(), val.data() + val.size(), std::get<0>(*this)); }
   void Minutes(std::string_view val) { auto result = std::from_chars(val.data(), val.data() + val.size(), std::get<1>(*this)); }
   void Seconds(std::string_view val) { auto result = std::from_chars(val.data(), val.data() + val.size(), std::get<2>(*this)); }

   template <std::floating_point param_ty = double>
   void DegreesAsDecimal(param_ty const& val) {
      param_ty tmp{ val };
      Degrees(std::floor(tmp));
      Minutes(std::floor((tmp -= Degrees()) *= 60));
      Seconds(std::round((tmp -= Minutes()) *= 60));
   }
private:
   void Parse(std::string const& para) {
      static auto constexpr special_char = [](char c) { return c == '°' || c == '"' || c == '\''; };

      std::string input{ para };
      std::ranges::replace_if(input, special_char, ' ');

      *this = { 0, 0, 0 };
      switch (auto ret = std::string_view{ input.begin(), input.end() }
         | std::views::split(' ')
         | std::views::filter([](auto token) { return !token.empty(); })
         | std::views::transform([](auto token) { return std::string_view{ token.begin(), token.end() }; })
         | std::ranges::to<std::vector>(); ret.size()) {
      case 3: Seconds(ret[2]);
         [[fallthrough]];
      case 2: Minutes(ret[1]);
         [[fallthrough]];
      case 1: Degrees(ret[0]);
         break;
      default: throw std::invalid_argument("invalid argument for parse Angle "s + para + "."s);
      }
   }
};
