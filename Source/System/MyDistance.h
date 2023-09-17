#pragma once


#include <iostream>
#include <cmath>
#include <string>
#include <numbers>
#include <type_traits>
#include <format>

using namespace std::string_literals;

enum class MyDistanceKind : int { without, millimeter, centimeter, meter, kilometer, miles, yards, foot, inchs };

template<typename ty>
inline constexpr bool always_false_distance = false;

template <typename ty>
concept my_param_distance = std::is_floating_point_v<ty> || (std::is_integral_v<ty> && !std::is_same_v<ty, bool>);

template <MyDistanceKind kind, MyDistanceKind oth_knd>
concept my_all_kind_without = (kind != MyDistanceKind::without && oth_knd != MyDistanceKind::without);

template <MyDistanceKind kind, MyDistanceKind oth_knd>
concept distance_kinds_convertible = !(kind == MyDistanceKind::without && oth_knd != MyDistanceKind::without) &&
                                     !(oth_knd == MyDistanceKind::without && kind != MyDistanceKind::without);


template <my_param_distance ty = double, MyDistanceKind kind = MyDistanceKind::without>
class MyDistance {
   friend std::ostream& operator << (std::ostream& out, MyDistance<ty, kind> const& dist) {
      return out << dist.Distance() << (kind == MyDistanceKind::without ? ""s : " "s + dist.unit());
      }
   
   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
     requires distance_kinds_convertible<kind, oth_knd>
   friend MyDistance<ty, kind> operator + (MyDistance<ty, kind> const& lhs, MyDistance<oth_ty, oth_knd> const& rhs) {
      return MyDistance<ty, kind> { lhs } += rhs; // .convert_to<kind, ty>();
      }

   template <my_param_distance oth_ty>
      requires (kind == MyDistanceKind::without)
   friend oth_ty operator + (MyDistance<ty, kind> const& lhs, oth_ty const& rhs) {
      oth_ty ret = static_cast<oth_ty>(lhs);
      return ret += rhs; 
      }


   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
      requires distance_kinds_convertible<kind, oth_knd>
   friend MyDistance<ty, kind> operator - (MyDistance<ty, kind> const& lhs, MyDistance<oth_ty, oth_knd> const& rhs) {
      return MyDistance<ty, kind> { lhs } -= rhs; // .convert_to<kind, ty>();
      }

   template <my_param_distance oth_ty>
      requires (kind == MyDistanceKind::without)
   friend oth_ty operator - (MyDistance<ty, kind> const& lhs, oth_ty const& rhs) {
      oth_ty ret = static_cast<oth_ty>(lhs);
      return ret -= rhs;
      }


   template <my_param_distance oth_ty>
   friend MyDistance<ty, kind> operator * (MyDistance<ty, kind> const& lhs, oth_ty const& rhs) {
      return MyDistance<ty, kind> { lhs } *= rhs;
      }

   template <my_param_distance oth_ty>
      requires std::is_convertible_v<oth_ty, ty>
   friend MyDistance<ty, kind> operator / (MyDistance<ty, kind> const& lhs, oth_ty const& rhs) {
      return MyDistance<ty, kind> { lhs } /= rhs;
      }
   
public:
   MyDistance(void) = default;


   MyDistance(my_param_distance auto const& val)  requires (kind == MyDistanceKind::without) {
      if constexpr (std::is_same_v<ty, decltype(val)>) theDistance = val;
      else {
         if constexpr (std::is_integral_v<ty> && !std::is_integral_v<decltype(val)>) theDistance = std::round(val);
         else theDistance = static_cast<ty>(val);
      }
   }

   explicit MyDistance(my_param_distance auto const& val) requires (kind != MyDistanceKind::without) {
      if constexpr (std::is_same_v<ty, decltype(val)>) theDistance = val;
      else {
         if constexpr (std::is_integral_v<ty> && !std::is_integral_v<decltype(val)>) theDistance = std::round(val);
         else theDistance = static_cast<ty>(val);
         }
      }


   //MyDistance(MyDistance const& other) : theDistance(other.Distance()) { }

   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
   MyDistance(MyDistance<oth_ty, oth_knd> const& other) {
      if constexpr (oth_knd == kind) {
         if constexpr (std::is_same_v<ty, oth_ty>) theDistance = other.theDistance();
         else theDistance = static_cast<ty>(other.theDistance());
         }
      else convert_from(other);
      }

   template <MyDistanceKind oth_knd, my_param_distance oth_ty, typename = std::enable_if_t<kind == oth_knd && std::is_same_v<ty, oth_ty>>>
   MyDistance(MyDistance<oth_ty, oth_knd>&& other) noexcept {
      std::swap(theDistance, other.theDistance);
      }


   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
   MyDistance& operator = (MyDistance<oth_ty, oth_knd> const& other) {
      convert_from(other);
      return *this;
      }

   
   template <MyDistanceKind oth_knd, my_param_distance oth_ty = ty, typename = std::enable_if_t<kind == oth_knd && std::is_same_v<ty, oth_ty>>>
   MyDistance& operator = (MyDistance&& other) noexcept {
      std::swap(theDistance, other.theDistance);
      return *this;
      }
    
   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
      requires distance_kinds_convertible<oth_knd, kind>
   auto operator <=> (MyDistance<oth_ty, oth_knd> const& other) const {
      const ty comp = other.convert_to<kind>().Distance();
      if constexpr (std::is_integral_v<ty>) return theDistance <=> comp;
      else {
         static constinit double epsilon = 1e-6; // Toleranz für Rundungsdifferenzen
         if (std::fabs(theDistance - comp) < epsilon) return std::strong_ordering::equal;
         else if (theDistance < comp) return std::strong_ordering::less;
         else return std::strong_ordering::greater;
         }
      }

   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
      requires distance_kinds_convertible<oth_knd, kind>
   bool operator == (MyDistance<oth_ty, oth_knd> const& ref)  const { return (*this <=> ref) == 0; }

   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
      requires distance_kinds_convertible<oth_knd, kind>
   bool operator != (MyDistance<oth_ty, oth_knd> const& ref)  const { return (*this <=> ref) != 0; }

   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
      requires distance_kinds_convertible<oth_knd, kind>
   bool operator <  (MyDistance<oth_ty, oth_knd> const& ref)  const { return (*this <=> ref) < 0; }

   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
      requires distance_kinds_convertible<oth_knd, kind>
   bool operator <= (MyDistance<oth_ty, oth_knd> const& ref)  const { return (*this <=> ref) <= 0; }

   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
      requires distance_kinds_convertible<oth_knd, kind>
   bool operator >  (MyDistance<oth_ty, oth_knd> const& ref)  const { return (*this <=> ref) > 0; }

   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
      requires distance_kinds_convertible<oth_knd, kind>
   bool operator >= (MyDistance<oth_ty, oth_knd> const& ref)  const { return (*this <=> ref) >= 0; }

   template <my_param_distance oth_ty, typename = std::enable_if_t<kind == MyDistanceKind::without && std::is_same_v<oth_ty, ty>>>
   operator oth_ty const& () const { return this->theDistance;  }

   template <my_param_distance oth_ty, typename = std::enable_if_t<kind == MyDistanceKind::without && !std::is_same_v<oth_ty, ty> && std::is_constructible_v<oth_ty, ty>>>
   operator oth_ty () const { return static_cast<oth_ty>(this->thisDistance); }

   operator std::string() const {
      if constexpr (kind == MyDistanceKind::without) return std::format("{}", theDistance);
      else return std::format("{} {}", theDistance, unit()); 
      }

   ///*
   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
      requires distance_kinds_convertible<oth_knd, kind>
   MyDistance& operator += (MyDistance<oth_ty, oth_knd> const& other) {
      if constexpr (kind == oth_knd) {
         if constexpr (std::is_same_v<ty, oth_ty>) theDistance += other.Distance();
         else theDistance += static_cast<ty>(other.Distance());
         }
      else {
         theDistance += create_from<oth_knd>(other).Distance();
         }
      return *this;
      }
   
   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
      requires distance_kinds_convertible<oth_knd, kind>
   MyDistance& operator -= (MyDistance<oth_ty, oth_knd> const& other) {
      if constexpr (kind == oth_knd) {
         if constexpr (std::is_same_v<ty, oth_ty>) theDistance -= other.Distance();
         else theDistance -= static_cast<ty>(other.Distance());
         }
      else {
         theDistance -= create_from<oth_knd>(other).Distance();
         }
      return *this;
      }
   
   MyDistance& operator *= (my_param_distance auto const& val) {
      if constexpr (std::is_same_v<ty, decltype(val)>) theDistance *= val;
      else theDistance *= static_cast<ty>(val);
      return *this;
      }

   MyDistance& operator /= (my_param_distance auto const& val) {
      if constexpr (std::is_same_v<ty, decltype(val)>) theDistance /= val;
      else theDistance /= static_cast<ty>(val);
      return *this;
      }

    template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty, typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty>>>
    ty const& Distance(void) const {
       if constexpr (kind == oth_knd) return theDistance;
       else return convert_to<oth_knd, ty>().Distance();
       }

    std::string unit(void) const {
       if constexpr      (kind == MyDistanceKind::millimeter) return "mm"s;
       else if constexpr (kind == MyDistanceKind::centimeter) return "cm"s;
       else if constexpr (kind == MyDistanceKind::meter)      return "m"s;
       else if constexpr (kind == MyDistanceKind::kilometer)  return "km"s;
       else if constexpr (kind == MyDistanceKind::miles)      return "mi"s;
       else if constexpr (kind == MyDistanceKind::yards)      return "yd"s;
       else if constexpr (kind == MyDistanceKind::foot)       return "ft"s;
       else if constexpr (kind == MyDistanceKind::inchs)      return "in"s;
       else if constexpr (kind == MyDistanceKind::without)    return ""s;
       else static_assert(always_false_distance<kind>, "unexpected unit for distance");
       }

    template <my_param_distance oth_ty = ty, typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty>>>
    void Distance(oth_ty const& val) {
      //using oth_ty = decltype(val);
      if constexpr (std::is_same_v<ty, oth_ty>) theDistance = val;
      else if constexpr (std::is_integral_v<ty> && !std::is_integral_v<oth_ty>) theDistance = std::round(val);
      else theDistance = static_cast<ty>(val);
      }

   void Distance(std::string_view val) { auto result = std::from_chars(val.data(), val.data() + val.size(), theDistance); }


   template<MyDistanceKind oth_knd, my_param_distance oth_ty = ty>
      requires distance_kinds_convertible<oth_knd, kind>
   MyDistance<ty, kind> create_from(MyDistance<oth_ty, oth_knd> const& other) const {
      MyDistance<ty, kind> ret;
      ret.convert_from(other);
      return ret;
      }


   template<MyDistanceKind oth_knd, my_param_distance oth_ty = ty>
      requires distance_kinds_convertible<oth_knd, kind>
   void convert_from(MyDistance<oth_ty, oth_knd> const& other) {
      if constexpr (kind == oth_knd) {
         if constexpr (std::is_same_v<ty, oth_ty>) Distance(other.Distance());
         else                                      Distance(static_cast<ty>(other.Distance()));
         }
      else if constexpr (kind == MyDistanceKind::without || oth_knd == MyDistanceKind::without) {
         static_assert(always_false_distance<kind>, "a distance without unit can't converted to or from a distance with unit");
         }
      else {
         if constexpr (kind == MyDistanceKind::millimeter) {
            if constexpr      (oth_knd == MyDistanceKind::centimeter) theDistance = set_value<ty>(other.Distance() *        10);
            else if constexpr (oth_knd == MyDistanceKind::meter)      theDistance = set_value<ty>(other.Distance() *     1'000);
            else if constexpr (oth_knd == MyDistanceKind::kilometer)  theDistance = set_value<ty>(other.Distance() * 1'000'000);
            // -------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::miles)      theDistance = set_value<ty>(other.Distance() * 1'609'344);
            else if constexpr (oth_knd == MyDistanceKind::yards)      theDistance = set_value<ty>(other.Distance() * 914.4);
            else if constexpr (oth_knd == MyDistanceKind::foot)       theDistance = set_value<ty>(other.Distance() * 304.8);
            else if constexpr (oth_knd == MyDistanceKind::inchs)      theDistance = set_value<ty>(other.Distance() * 25.4);
            else static_assert(always_false_distance<oth_knd>, "unexpected unit for distance");
            }
         else if constexpr (kind == MyDistanceKind::centimeter) {
            if constexpr      (oth_knd == MyDistanceKind::millimeter) theDistance = set_value<ty>(other.Distance() /      10);
            else if constexpr (oth_knd == MyDistanceKind::meter)      theDistance = set_value<ty>(other.Distance() *     100);
            else if constexpr (oth_knd == MyDistanceKind::kilometer)  theDistance = set_value<ty>(other.Distance() * 100'000);
            // -------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::miles)      theDistance = set_value<ty>(other.Distance() * 160'934.4);
            else if constexpr (oth_knd == MyDistanceKind::yards)      theDistance = set_value<ty>(other.Distance() *      91.44);
            else if constexpr (oth_knd == MyDistanceKind::foot)       theDistance = set_value<ty>(other.Distance() *      30.48);
            else if constexpr (oth_knd == MyDistanceKind::inchs)      theDistance = set_value<ty>(other.Distance() *       2.54);
            else static_assert(always_false_distance<oth_knd>, "unexpected unit for distance");
            }
         else if constexpr (kind == MyDistanceKind::meter) {
            if constexpr      (oth_knd == MyDistanceKind::millimeter) theDistance = set_value<ty>(other.Distance() / 1'000);
            else if constexpr (oth_knd == MyDistanceKind::centimeter) theDistance = set_value<ty>(other.Distance() /   100);
            else if constexpr (oth_knd == MyDistanceKind::kilometer)  theDistance = set_value<ty>(other.Distance() * 1'000);
            // -------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::miles)      theDistance = set_value<ty>(other.Distance() * 1'609.34);
            else if constexpr (oth_knd == MyDistanceKind::yards)      theDistance = set_value<ty>(other.Distance() * 0.91439997073920623088319997081);
            else if constexpr (oth_knd == MyDistanceKind::foot)       theDistance = set_value<ty>(other.Distance() * 0.3048);
            else if constexpr (oth_knd == MyDistanceKind::inchs)      theDistance = set_value<ty>(other.Distance() * 0.0254);
            else static_assert(always_false_distance<oth_knd>, "unexpected unit for distance");
            }
         else if constexpr (kind == MyDistanceKind::kilometer) {
            if constexpr      (oth_knd == MyDistanceKind::millimeter) theDistance = set_value<ty>(other.Distance() / 1'000'000);
            else if constexpr (oth_knd == MyDistanceKind::centimeter) theDistance = set_value<ty>(other.Distance() /   100'000);
            else if constexpr (oth_knd == MyDistanceKind::meter)      theDistance = set_value<ty>(other.Distance() /     1'000);
            // -------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::miles)      theDistance = set_value<ty>(other.Distance() * 1.60934);
            else if constexpr (oth_knd == MyDistanceKind::yards)      theDistance = set_value<ty>(other.Distance() * 0.0009144);
            else if constexpr (oth_knd == MyDistanceKind::foot)       theDistance = set_value<ty>(other.Distance() * 0.0003048);
            else if constexpr (oth_knd == MyDistanceKind::inchs)      theDistance = set_value<ty>(other.Distance() * 0.0000254);
            else static_assert(always_false_distance<oth_knd>, "unexpected unit for distance");
            }
         else if constexpr (kind == MyDistanceKind::miles) {
            if constexpr      (oth_knd == MyDistanceKind::millimeter) theDistance = set_value<ty>(other.Distance() / 1'609'344);
            else if constexpr (oth_knd == MyDistanceKind::centimeter) theDistance = set_value<ty>(other.Distance() /   160'934.4);
            else if constexpr (oth_knd == MyDistanceKind::meter)      theDistance = set_value<ty>(other.Distance() /     1'609.344);
            else if constexpr (oth_knd == MyDistanceKind::kilometer)  theDistance = set_value<ty>(other.Distance() /         1.609344);
            // -------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::yards)      theDistance = set_value<ty>(other.Distance() /           1'760);
            else if constexpr (oth_knd == MyDistanceKind::foot)       theDistance = set_value<ty>(other.Distance() / (     3 * 1'760));
            else if constexpr (oth_knd == MyDistanceKind::inchs)      theDistance = set_value<ty>(other.Distance() / (12 * 3 * 1'760));
            else static_assert(always_false_distance<oth_knd>, "unexpected unit for distance");
            }
         else if constexpr (kind == MyDistanceKind::yards) {
            if constexpr      (oth_knd == MyDistanceKind::millimeter) theDistance = set_value<ty>(other.Distance() / 914.4);
            else if constexpr (oth_knd == MyDistanceKind::centimeter) theDistance = set_value<ty>(other.Distance() / 91.44);
            else if constexpr (oth_knd == MyDistanceKind::meter)      theDistance = set_value<ty>(other.Distance() * 1.09361329833770779452749558124);
            else if constexpr (oth_knd == MyDistanceKind::kilometer)  theDistance = set_value<ty>(other.Distance() * 1'093.61);
            // -------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::miles)      theDistance = set_value<ty>(other.Distance() *         1'760);
            else if constexpr (oth_knd == MyDistanceKind::foot)       theDistance = set_value<ty>(other.Distance() /       3);
            else if constexpr (oth_knd == MyDistanceKind::inchs)      theDistance = set_value<ty>(other.Distance() / (12 * 3));
            else  static_assert(always_false_distance<oth_knd>, "unexpected unit for distance");
            }
         else if constexpr (kind == MyDistanceKind::foot) {
            if constexpr      (oth_knd == MyDistanceKind::millimeter) theDistance = set_value<ty>(other.Distance() / 304.8);
            else if constexpr (oth_knd == MyDistanceKind::centimeter) theDistance = set_value<ty>(other.Distance() / 30.48);
            else if constexpr (oth_knd == MyDistanceKind::meter)      theDistance = set_value<ty>(other.Distance() * 3.28084);
            else if constexpr (oth_knd == MyDistanceKind::kilometer)  theDistance = set_value<ty>(other.Distance() * 3'280.84);
            // -------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::miles)      theDistance = set_value<ty>(other.Distance() * (   3 * 1'760));
            else if constexpr (oth_knd == MyDistanceKind::yards)      theDistance = set_value<ty>(other.Distance() *     3);
            else if constexpr (oth_knd == MyDistanceKind::inchs)      theDistance = set_value<ty>(other.Distance() /  12);
            else  static_assert(always_false_distance<oth_knd>, "unexpected unit for distance");
            }
         else if constexpr (kind == MyDistanceKind::inchs) {
            if constexpr      (oth_knd == MyDistanceKind::millimeter) theDistance = set_value<ty>(other.Distance() / 25.4);
            else if constexpr (oth_knd == MyDistanceKind::centimeter) theDistance = set_value<ty>(other.Distance() / 2.54);
            else if constexpr (oth_knd == MyDistanceKind::meter)      theDistance = set_value<ty>(other.Distance() * 39.3701);
            else if constexpr (oth_knd == MyDistanceKind::kilometer)  theDistance = set_value<ty>(other.Distance() * 39370.1);
            // -------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::miles)      theDistance = set_value<ty>(other.Distance() * (12 * 3 * 1'760));
            else if constexpr (oth_knd == MyDistanceKind::yards)      theDistance = set_value<ty>(other.Distance() * (12 * 3        ));
            else if constexpr (oth_knd == MyDistanceKind::foot)       theDistance = set_value<ty>(other.Distance() *  12            );
            else  static_assert(always_false_distance<oth_knd>, "unexpected unit for distance");
            }
         else {
            static_assert(always_false_distance<kind>, "unexpected unit for distance");
            }
         }
      }


   template< MyDistanceKind oth_knd, my_param_distance oth_ty = ty>
   std::conditional_t<(kind == oth_knd && std::is_same_v<ty, oth_ty>), MyDistance<oth_ty, oth_knd> const&, MyDistance<oth_ty, oth_knd>> convert_to() const {
      if constexpr (kind == oth_knd) {
         if constexpr (std::is_same_v<ty, oth_ty>) return *this;
         else {
            if constexpr (std::is_integral_v<oth_ty> && !std::is_integral_v<ty>)  
               return MyDistance<oth_ty, oth_knd>(static_cast<oth_ty>(std::round(Distance())));
            else  return MyDistance<oth_ty, oth_knd>(static_cast<oth_ty>(Distance()));
            }
         }                                      
      else if constexpr (kind == MyDistanceKind::without || oth_knd == MyDistanceKind::without) {
         static_assert(always_false_distance<kind>, "a distance without unit can't be converted to or from a distance with unit");
         }
      else {
         if constexpr (kind == MyDistanceKind::millimeter) {
            if constexpr      (oth_knd == MyDistanceKind::centimeter) return MyDistance<oth_ty, oth_knd>(theDistance /        10);
            else if constexpr (oth_knd == MyDistanceKind::meter)      return MyDistance<oth_ty, oth_knd>(theDistance /     1'000);
            else if constexpr (oth_knd == MyDistanceKind::kilometer)  return MyDistance<oth_ty, oth_knd>(theDistance / 1'000'000);
            // ----------------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::miles)      return MyDistance<oth_ty, oth_knd>(theDistance * 0.000000621371);
            else if constexpr (oth_knd == MyDistanceKind::yards)      return MyDistance<oth_ty, oth_knd>(theDistance * 0.00109361);
            else if constexpr (oth_knd == MyDistanceKind::foot)       return MyDistance<oth_ty, oth_knd>(theDistance * 0.00328084);
            else if constexpr (oth_knd == MyDistanceKind::inchs)      return MyDistance<oth_ty, oth_knd>(theDistance * 0.0393701);
            else static_assert(always_false_distance<oth_knd>, "unexpected unit for distance in convert_to");
            }
         else if constexpr (kind == MyDistanceKind::centimeter) {
            if constexpr      (oth_knd == MyDistanceKind::millimeter) return MyDistance<oth_ty, oth_knd>(theDistance *      10);
            else if constexpr (oth_knd == MyDistanceKind::meter)      return MyDistance<oth_ty, oth_knd>(theDistance /     100);
            else if constexpr (oth_knd == MyDistanceKind::kilometer)  return MyDistance<oth_ty, oth_knd>(theDistance / 100'000);
            // ----------------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::miles)      return MyDistance<oth_ty, oth_knd>(theDistance / 160934.4);
            else if constexpr (oth_knd == MyDistanceKind::yards)      return MyDistance<oth_ty, oth_knd>(theDistance / 91.44);
            else if constexpr (oth_knd == MyDistanceKind::foot)       return MyDistance<oth_ty, oth_knd>(theDistance / 30.48);
            else if constexpr (oth_knd == MyDistanceKind::inchs)      return MyDistance<oth_ty, oth_knd>(theDistance / 2.54);
            else static_assert(always_false_distance<oth_knd>, "unexpected unit for distance in convert_to");
            }
         else if constexpr (kind == MyDistanceKind::meter) {
            if constexpr      (oth_knd == MyDistanceKind::millimeter) return MyDistance<oth_ty, oth_knd>(theDistance * 1'000);
            else if constexpr (oth_knd == MyDistanceKind::centimeter) return MyDistance<oth_ty, oth_knd>(theDistance *   100);
            else if constexpr (oth_knd == MyDistanceKind::kilometer)  return MyDistance<oth_ty, oth_knd>(theDistance / 1'000);
            // ----------------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::miles)      return MyDistance<oth_ty, oth_knd>(theDistance / 1609.34);
            else if constexpr (oth_knd == MyDistanceKind::yards)      return MyDistance<oth_ty, oth_knd>(theDistance * 1.09361329833770779452749558124);
            else if constexpr (oth_knd == MyDistanceKind::foot)       return MyDistance<oth_ty, oth_knd>(theDistance / 0.3048);
            else if constexpr (oth_knd == MyDistanceKind::inchs)      return MyDistance<oth_ty, oth_knd>(theDistance / 0.0254);
            else static_assert(always_false_distance<oth_knd>, "unexpected unit for distance in convert_to");
            }
         else if constexpr (kind == MyDistanceKind::kilometer) {
            if constexpr      (oth_knd == MyDistanceKind::millimeter) return MyDistance<oth_ty, oth_knd>(theDistance * 1'000'000);
            else if constexpr (oth_knd == MyDistanceKind::centimeter) return MyDistance<oth_ty, oth_knd>(theDistance *   100'000);
            else if constexpr (oth_knd == MyDistanceKind::meter)      return MyDistance<oth_ty, oth_knd>(theDistance *     1'000);
            // ----------------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::miles)      return MyDistance<oth_ty, oth_knd>(theDistance * 1.60934);
            else if constexpr (oth_knd == MyDistanceKind::yards)      return MyDistance<oth_ty, oth_knd>(theDistance * 1093.61);
            else if constexpr (oth_knd == MyDistanceKind::foot)       return MyDistance<oth_ty, oth_knd>(theDistance * 3280.84);
            else if constexpr (oth_knd == MyDistanceKind::inchs)      return MyDistance<oth_ty, oth_knd>(theDistance * 39370.1);
            else  static_assert(always_false_distance<oth_knd>, "unexpected unit for distance in convert_to");
            }
         else if constexpr (kind == MyDistanceKind::miles) {
            if constexpr      (oth_knd == MyDistanceKind::millimeter) return MyDistance<oth_ty, oth_knd>(theDistance * 1'609'344);
            else if constexpr (oth_knd == MyDistanceKind::centimeter) return MyDistance<oth_ty, oth_knd>(theDistance *   160'934.4);
            else if constexpr (oth_knd == MyDistanceKind::meter)      return MyDistance<oth_ty, oth_knd>(theDistance *     1'609.34);
            else if constexpr (oth_knd == MyDistanceKind::kilometer)  return MyDistance<oth_ty, oth_knd>(theDistance *         1.60934);
            // -------------------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::yards)      return MyDistance<oth_ty, oth_knd>(theDistance *           1'760);
            else if constexpr (oth_knd == MyDistanceKind::foot)       return MyDistance<oth_ty, oth_knd>(theDistance * (     3 * 1'760));
            else if constexpr (oth_knd == MyDistanceKind::inchs)      return MyDistance<oth_ty, oth_knd>(theDistance * (12 * 3 * 1'760));
            else static_assert(always_false_distance<oth_knd>, "unexpected unit for distance in convert_to");
            }
         else if constexpr (kind == MyDistanceKind::yards) {
            if constexpr      (oth_knd == MyDistanceKind::millimeter) return MyDistance<oth_ty, oth_knd>(theDistance * 914.4);
            else if constexpr (oth_knd == MyDistanceKind::centimeter) return MyDistance<oth_ty, oth_knd>(theDistance * 91.44);
            else if constexpr (oth_knd == MyDistanceKind::meter)      return MyDistance<oth_ty, oth_knd>(theDistance * 0.91439997073920623088319997081);
            else if constexpr (oth_knd == MyDistanceKind::kilometer)  return MyDistance<oth_ty, oth_knd>(theDistance * 0.0009144);
            // ----------------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::miles)      return MyDistance<oth_ty, oth_knd>(theDistance /          1'760);
            else if constexpr (oth_knd == MyDistanceKind::foot)       return MyDistance<oth_ty, oth_knd>(theDistance *       3       );
            else if constexpr (oth_knd == MyDistanceKind::inchs)      return MyDistance<oth_ty, oth_knd>(theDistance * (12 * 3       ));
            else  static_assert(always_false_distance<oth_knd>, "unexpected unit for distance in convert_to");
            }
         else if constexpr (kind == MyDistanceKind::foot) {
            if constexpr      (oth_knd == MyDistanceKind::millimeter) return MyDistance<oth_ty, oth_knd>(theDistance * 304.8);
            else if constexpr (oth_knd == MyDistanceKind::centimeter) return MyDistance<oth_ty, oth_knd>(theDistance * 30.48);
            else if constexpr (oth_knd == MyDistanceKind::meter)      return MyDistance<oth_ty, oth_knd>(theDistance * 0.3048);
            else if constexpr (oth_knd == MyDistanceKind::kilometer)  return MyDistance<oth_ty, oth_knd>(theDistance * 0.0003048);
            // ----------------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::miles)      return MyDistance<oth_ty, oth_knd>(theDistance / (    3 * 1'760));
            else if constexpr (oth_knd == MyDistanceKind::yards)      return MyDistance<oth_ty, oth_knd>(theDistance /      3);
            else if constexpr (oth_knd == MyDistanceKind::inchs)      return MyDistance<oth_ty, oth_knd>(theDistance * 12);
            else static_assert(always_false_distance<oth_knd>, "unexpected unit for distance in convert_to");
            }
         else if constexpr (kind == MyDistanceKind::inchs) {
            if constexpr      (oth_knd == MyDistanceKind::millimeter) return MyDistance<oth_ty, oth_knd>(theDistance * 25.4);
            else if constexpr (oth_knd == MyDistanceKind::centimeter) return MyDistance<oth_ty, oth_knd>(theDistance * 2.54);
            else if constexpr (oth_knd == MyDistanceKind::meter)      return MyDistance<oth_ty, oth_knd>(theDistance * 0.0254);
            else if constexpr (oth_knd == MyDistanceKind::kilometer)  return MyDistance<oth_ty, oth_knd>(theDistance * 0.0000254);
            // ----------------------------------------------------------------------------------------------------------------
            else if constexpr (oth_knd == MyDistanceKind::miles)      return MyDistance<oth_ty, oth_knd>(theDistance / (12 * 3 * 1'760));
            else if constexpr (oth_knd == MyDistanceKind::yards)      return MyDistance<oth_ty, oth_knd>(theDistance / (12 * 3        ));
            else if constexpr (oth_knd == MyDistanceKind::foot)       return MyDistance<oth_ty, oth_knd>(theDistance /  12);
            else  static_assert(always_false_distance<oth_knd>, "unexpected unit for distance in convert_to");
            }
         else static_assert(always_false_distance<kind>, "unexpected unit for distance in convert_to");
         }
      }

   private:
      /// method to convert different types with rules
      template <my_param_distance ret_ty, typename other_ty>
      auto set_value(other_ty&& val) const -> std::conditional_t<std::is_same_v<other_ty, ret_ty>, ret_ty&&, ret_ty>  {
         if constexpr (std::is_integral_v<ret_ty> && !std::is_integral_v<other_ty>) {
            return static_cast<ret_ty>(std::round(val));
            }
         else if constexpr (!std::is_same_v<ret_ty, other_ty>) {
            return static_cast<ret_ty>(val);
            }
         else {
            return std::move(val);
            }
         }

      ty theDistance;
   };

// ----------------------------------------------------------------------------------
/**  namespace to isolate the literals for distances 
*/
namespace my_distance_literals {

inline auto operator ""_millimeter(long double val) {
   return MyDistance<double, MyDistanceKind::millimeter> { val };
   }

inline auto operator ""_centimeter(long double val) {
   return MyDistance<double, MyDistanceKind::centimeter>(val);
   }

inline auto operator ""_meter(long double val) {
   return MyDistance<double, MyDistanceKind::meter>(val);
   }

inline auto operator ""_kilometer(long double val) {
   return MyDistance<double, MyDistanceKind::kilometer>(val);
   }

inline MyDistance<double, MyDistanceKind::miles> operator""_mile(long double val) {
   return MyDistance<double, MyDistanceKind::miles>(val);
   }

inline MyDistance<double, MyDistanceKind::yards> operator""_yard(long double val) {
   return MyDistance<double, MyDistanceKind::yards>(val);
   }

inline MyDistance<double, MyDistanceKind::foot> operator""_foot(long double val) {
   return MyDistance<double, MyDistanceKind::foot>(val);
   }

inline MyDistance<double, MyDistanceKind::inchs> operator""_inch(long double val) {
   return MyDistance<double, MyDistanceKind::inchs>(val);
   }

} // end of namespace my_distance_literals

