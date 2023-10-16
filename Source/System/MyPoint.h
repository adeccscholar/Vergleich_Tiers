#pragma once
/*

Selektoren sind fertig


*/


#if __has_include("MyPoint.h")
   #include "MyAngle.h"
   #include "MyDistance.h"
#elif __has_include("System/MyPoint.h")
   #include "System/MyAngle.h"
   #include "System/MyDistance.h"
#endif

#include <iostream>
#include <sstream>
#include <array>
#include <utility>
#include <stdexcept>
#include <cmath>
#include <string>
#include <algorithm>
#include <numbers>
#include <type_traits>
#include <initializer_list>
#include <format>
#include <numeric>
#include <ranges>



template <my_param_distance ty, int SIZE, MyDistanceKind kind = MyDistanceKind::without, typename = std::enable_if_t<SIZE == 2 || SIZE == 3>>
class MyPoint : public std::array<MyDistance<ty, kind>, SIZE> {
   
   friend std::ostream& operator << (std::ostream& out, MyPoint<ty, SIZE, kind> const& point) {
      out << "(" << *point.cbegin();
      std::for_each(point.cbegin() + 1, point.cend(), [&out](auto const& val) { out << ", " << val; });
      //for (size_t i = 0; i < SIZE; ++i) out << (i > 0 ? ", " : "") << point[i];
      return out << ")";
      }

   friend std::istream& operator >> (std::istream& in, MyPoint<ty, SIZE, kind>& point) {
      char c;
      in >> c;
      for (size_t i = 0; i < SIZE; ++i) in >> point[i] >> c;
      return in;
      }
 
   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty, 
      typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty> && distance_kinds_convertible<kind, oth_knd>>>
   friend MyPoint<ty, SIZE, kind> operator + (MyPoint<ty, SIZE, kind> const& a, MyPoint<oth_ty, SIZE, oth_knd> const& b) {
      return MyPoint<ty, SIZE, kind>(a) += b;
      }

   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty,
      typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty>&& distance_kinds_convertible<kind, oth_knd>>>
   friend MyPoint<ty, SIZE, kind> operator - (MyPoint<ty, SIZE, kind> const& a, MyPoint<oth_ty, SIZE, oth_knd> const& b) {
      return MyPoint<ty, SIZE, kind>(a) -= b;
      }

   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty,
      typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty>>>
   friend MyPoint<ty, SIZE, kind> operator * (MyPoint<ty, SIZE, kind> const& a, oth_ty const& b) {
      return MyPoint<ty, SIZE, kind>(a) *= b;
      }

   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty,
      typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty>>>
   friend MyPoint<ty, SIZE, kind> operator * (oth_ty const& a, MyPoint<ty, SIZE, kind> const& b) {
      return MyPoint<ty, SIZE, kind>(b) *= a;
      }

   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty,
      typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty>>>
   friend MyPoint<ty, SIZE, kind> operator / (MyPoint<ty, SIZE, kind> const& a, oth_ty const& b) {
      return MyPoint<ty, SIZE, kind>(a) /= b;
      }

public:
   using value_type = ty;

   constexpr MyPoint(void) = default;

   // Kopierkonstruktor für MyPoint mit gleichen Ty
   MyPoint(MyPoint<ty, SIZE, kind> const& other) : std::array<MyDistance<ty, kind>, SIZE>(other) {}

   // Kopierkonstruktor für abweichenden Ty
   //template <typename oth_ty, typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty>>>

   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty, 
      typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty> && distance_kinds_convertible<kind, oth_knd>>>
   MyPoint(MyPoint<oth_ty, SIZE, oth_knd> const& other) {
      std::transform(other.begin(), other.end(), this->begin(), [](auto const& val) { return val.convert_to<kind, ty>(); });
      }


   /*
   template <typename... Args, typename = std::enable_if_t<sizeof...(Args) == SIZE &&
                                              std::conjunction_v<std::is_same<Args, ty>...>>>
   MyPoint(Args&&... args) : std::array<MyDistance<ty, kind>, SIZE>{std::forward<Args>(args)...} {}
   */
   
   template <typename... Args, typename dummy = void, 
                               typename = std::enable_if_t<sizeof...(Args) == SIZE>> // &&
                                        //  std::conjunction_v<!std::is_same<Args, ty>, std::is_convertible<Args, ty>...>>>
   MyPoint(Args&&... args) : std::array<MyDistance<ty, kind>, SIZE> { static_cast<MyDistance<ty, kind>>(std::forward<Args>(args))...} {}
   
   //--------------------------------------------------------
   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty>
   MyPoint(std::vector<MyDistance<oth_ty, oth_knd>> const& vec) {
      if (vec.size() != SIZE) { throw std::invalid_argument("Vector size does not match MyPoint size."); }
      if constexpr (oth_knd == kind && std::is_same_v<oth_ty, ty>) std::copy(vec.begin(), vec.end(), this->begin());
      else std::transform(vec.begin(), vec.end(), this->begin(), [](auto const & val) { return val.convert_to<kind, ty>(); });
      }


   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty>
   MyPoint(std::span<MyDistance<oth_ty, oth_knd>> span) {
      if (span.size() != SIZE) { throw std::invalid_argument("size of std::span don't match wih MyPoint size."); }
      if constexpr (oth_knd == kind && std::is_same_v<oth_ty, ty>) std::copy(span.begin(), span.end(), this->begin());
      else std::transform(span.begin(), span.end(), this->begin(), [](auto const& val) { return val.convert_to<kind, ty>(); });
      }

   //------------------------------------------------------------------
   /*
   MyPoint(std::string const& text) {
      std::istringstream ss(text);
      ss >> *this;
      if (!ss) throw std::invalid_argument("invalid input");
      }
   */


   // ------------------------------------------------------------------
   // Selektoren der Klasse
   // ------------------------------------------------------------------
   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty>
   std::enable_if_t<std::is_convertible_v<ty, oth_ty>, 
        std::conditional_t<(oth_knd == kind) && std::is_same_v<ty, oth_ty>, MyDistance<ty, kind> const&, MyDistance<oth_ty, oth_knd>>> X() const {
      if constexpr (kind == oth_knd && std::is_same_v<oth_ty, ty>) return (*this)[0];
      else return (*this)[0].convert_to<oth_knd, oth_ty>();
      }


   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty>
   std::enable_if_t<std::is_convertible_v<ty, oth_ty>,
      std::conditional_t<(oth_knd == kind) && std::is_same_v<ty, oth_ty>, MyDistance<ty, kind> const&, MyDistance<oth_ty, oth_knd>>> Y() const {
      if constexpr (kind == oth_knd && std::is_same_v<oth_ty, ty>) return (*this)[1];
      else return (*this)[1].convert_to<oth_knd, oth_ty>();
      }



   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty>
   std::enable_if_t<SIZE == 3 && std::is_convertible_v<ty, oth_ty>,
         std::conditional_t<(oth_knd == kind) && std::is_same_v<ty, oth_ty>, MyDistance<ty, kind> const&, MyDistance<oth_ty, oth_knd>>> Z() const {
         if constexpr (kind == oth_knd && std::is_same_v<oth_ty, ty>) return (*this)[2];
         else return (*this)[2].convert_to<oth_knd, oth_ty>();
      }

   // ------------------------------------------------------------------
   // Manipulatoren der Klasse
   // ------------------------------------------------------------------
   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
      requires std::is_convertible_v<oth_ty, ty> && distance_kinds_convertible<oth_knd, kind>
   void X(MyDistance<oth_ty, oth_knd> const& x) {
      (*this)[0] = x;
      }

   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
      requires std::is_convertible_v<oth_ty, ty> && distance_kinds_convertible<oth_knd, kind>
   void Y(MyDistance<oth_ty, oth_knd> const& y) {
      (*this)[1] = y;
      }

   template <MyDistanceKind oth_knd, my_param_distance oth_ty>
      requires (SIZE == 3) && std::is_convertible_v<oth_ty, ty> && distance_kinds_convertible<oth_knd, kind>
   void Z(MyDistance<oth_ty, oth_knd> const& z) {
      (*this)[2] = z;
      }


   // ------------------------------------------------------------------
   // Berechnungen der Klasse
   // ------------------------------------------------------------------
   MyDistance<ty, kind> Magnitude() const {
      return MyDistance<ty, kind>(std::sqrt(std::accumulate(this->begin(), this->end(), ty{ }, [](ty sum, MyDistance<ty, kind> const& component) {
         return sum + component.Distance() * component.Distance(); })));
      }


   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty,
      typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty>&& distance_kinds_convertible<kind, oth_knd>>>
   MyDistance<ty, kind> Distance(MyPoint<oth_ty, SIZE, oth_knd> const& other) const {
      auto Diffs = std::views::zip(*this, other) 
                     | std::views::transform([](auto const& p) {
                           auto diff = std::get<1>(p).Distance<kind>() - std::get<0>(p).Distance<kind>();
                           return diff * diff; });
      return MyDistance<ty, kind>(std::sqrt(std::accumulate(Diffs.begin(), Diffs.end(), 0.0)));
      }


   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty,
      typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty>&& distance_kinds_convertible<kind, oth_knd>>>
   MyPoint<ty, SIZE, kind> Center(MyPoint<oth_ty, SIZE, oth_knd> const& other) const {
      return std::views::zip(*this, other) 
                     | std::views::transform([](auto const& p) { return (std::get<0>(p) + std::get<1>(p)) / 2;  }) 
                     | std::ranges::to<std::vector>();
      }

   MyPoint<ty, SIZE, kind> UnitVector() const {
      return *this / Magnitude().Distance();
      }

   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty,
      typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty>&& distance_kinds_convertible<kind, oth_knd>>>
   MyDistance<ty, kind> ScalarProduct(MyPoint<oth_ty, SIZE, oth_knd> const& other) const {
      auto products = std::views::zip(*this, other) | std::views::transform([](auto const& pair) {
         if constexpr (std::is_same_v<oth_ty, ty>) return std::get<0>(pair).Distance<kind>() * std::get<1>(pair).Distance<kind>();
         }) | std::ranges::to<std::vector>();
      auto ret = std::accumulate(products.begin(), products.end(), static_cast<ty>(0.0));
      return MyDistance<ty, kind>(ret);
      }


   template <MyAngleKind angle_knd = MyAngleKind::degree, MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty>
      requires std::is_convertible_v<oth_ty, ty> && distance_kinds_convertible<kind, oth_knd>
   MyAngle<double, angle_knd> Angle(MyPoint<oth_ty, SIZE, oth_knd> const& other) const {
      if constexpr (std::is_integral_v<ty> && std::is_integral_v<oth_ty>) {
         auto ret = std::acos(1.0 * ScalarProduct(other).Distance<kind>() / (Magnitude().Distance<kind>() * other.Magnitude().Distance<kind>()));
         return MyAngle<double, MyAngleKind::radian>(ret);
         }
      else {
         auto ret = std::acos(ScalarProduct(other).Distance<kind>() / (Magnitude().Distance<kind>() * other.Magnitude().Distance<kind>()));
         return MyAngle<double, MyAngleKind::radian>(ret);
         }
      }

   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty, typename = std::enable_if_t<SIZE == 3>>
     requires std::is_convertible_v<oth_ty, ty> && distance_kinds_convertible<kind, oth_knd>
   MyPoint<ty, SIZE, kind> CrossProduct(MyPoint<oth_ty, SIZE, oth_knd> const& other) const {
      return { (*this)[1] * other[2].Distance<kind>() - (*this)[2] * other[1].Distance<kind>(),
               (*this)[2] * other[0].Distance<kind>() - (*this)[0] * other[2].Distance<kind>(),
               (*this)[0] * other[1].Distance<kind>() - (*this)[1] * other[0].Distance<kind>() };
      }
/*
* 
* 

    std::pair<double, std::array<double, SIZE - 1>> getAzimuthInclinationAndDistance() const {
        std::array<double, SIZE - 1> angles;
        double distance = 0.0;

        if constexpr (SIZE == 2) {
            // 2D-Berechnung für Azimut
            angles[0] = std::atan2((*this)[1], (*this)[0]);
            distance = std::sqrt((*this)[0] * (*this)[0] + (*this)[1] * (*this)[1]);
        }

        if constexpr (SIZE == 3) {
            // 3D-Berechnung für Azimut
            angles[0] = std::atan2((*this)[1], (*this)[0]);

            // 3D-Berechnung für Neigung
            double xyLength = std::sqrt((*this)[0] * (*this)[0] + (*this)[1] * (*this)[1]);
            angles[1] = std::atan2((*this)[2], xyLength);

            // Berechnung der Distanz
            distance = std::sqrt((*this)[0] * (*this)[0] + (*this)[1] * (*this)[1] + (*this)[2] * (*this)[2]);
        }

        return std::make_pair(distance, angles);
    }

    static MyPoint fromAzimuthInclinationAndDistance(double azimuth, double inclination, double distance) {
        MyPoint result;

        if constexpr (SIZE == 2) {
            // 2D-Berechnung für Punkt
            result[0] = distance * std::cos(azimuth);
            result[1] = distance * std::sin(azimuth);
        }

        if constexpr (SIZE == 3) {
            // 3D-Berechnung für Punkt
            result[0] = distance * std::cos(azimuth) * std::cos(inclination);
            result[1] = distance * std::sin(azimuth) * std::cos(inclination);
            result[2] = distance * std::sin(inclination);
        }

        return result;
    }

*/



   // --------------- !!! BAUSTELLE -----------------------

   //auto Angle(void) const {
      /*
      std::array<ty, SIZE - 1> angles;
      //std::ranges::transform(std::next(this->begin()), this->end(), angles.begin(), [x = (*this)[0]](ty const& y) { return std::atan2(y, x); });
      std::ranges::transform(this->begin() + 1, this->end(), angles.begin(), [x = (*this)[0]](ty const& y) { return std::atan2(y, x) * (180.0 / std::numbers::phi_v<double>); });
      return angles;
      */
   //   std::array<ty, SIZE - 1> angles;
   //   std::ranges::transform(this->begin() + 1, this->end(), angles.begin(), [x = (*this)[0]](ty const& y) { return std::atan2(y, x); });

      // Umwandeln der Winkel von Radianen in Grad
   //   std::array<ty, SIZE - 1> anglesInDegrees;
   //   std::transform(angles.begin(), angles.end(), anglesInDegrees.begin(), [](ty const& angle) { return angle * (180.0 / std::numbers::pi_v<ty>); });

   //   return anglesInDegrees;
   //}


   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty>
   std::enable_if_t<std::is_convertible_v<ty, oth_ty>,
      std::conditional_t<(oth_knd == kind) && std::is_same_v<ty, oth_ty>, MyPoint<ty, SIZE, kind> const&, MyPoint<oth_ty, SIZE, oth_knd>>> convert_to() const {
      if constexpr ((oth_knd == kind) && std::is_same_v<ty, oth_ty>) return *this;
      else return MyPoint<oth_ty, SIZE, oth_knd>(*this);
      }
   
   // ------------------------------------------------------------------------------------------------------------
   MyPoint<ty, SIZE, kind> operator -() const {
      MyPoint<ty, SIZE, kind> ret;
      std::transform(this->begin(), this->end(), ret.begin(), [](auto const& val) { return -val; });
      //for (size_t idx = 0; auto const& val : *this) ret[idx++] = -val;
      return ret;
      } 

   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty, 
         typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty> && distance_kinds_convertible<kind, oth_knd>>>
   MyPoint<ty, SIZE, kind>& operator += (const MyPoint<oth_ty, SIZE, oth_knd>& other) {
      *this = std::views::zip(*this, other) | std::views::transform([](auto const& pair) { 
                                                  return std::get<0>(pair) + std::get<1>(pair);
                                                  }) | std::ranges::to<std::vector>();
      return *this;
      }

   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty,
      typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty>&& distance_kinds_convertible<kind, oth_knd>>>
   MyPoint<ty, SIZE, kind>& operator -= (const MyPoint<oth_ty, SIZE, oth_knd>& other) {
      *this = std::views::zip(*this, other) | std::views::transform([](auto const& pair) {
                                                  return std::get<0>(pair) - std::get<1>(pair);
                                                  }) | std::ranges::to<std::vector>();
         return *this;
      }

   template <my_param_distance oth_ty = ty, typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty>>>
   MyPoint<ty, SIZE, kind>& operator *= (oth_ty const& scalar) {
      if constexpr (std::is_same_v<oth_ty, ty>) { for(auto& val : *this) { val *= scalar; } }
      else if constexpr (std::is_same_v<decltype(std::declval<ty>() * scalar), ty>) { for (auto& val : *this) { val *= scalar; } }
      else { for (auto& val : *this) { val = static_cast<ty>(val * scalar); } }
      return *this;
      }

   template <my_param_distance oth_ty = ty, typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty>>>
   MyPoint<ty, SIZE, kind>& operator /= (oth_ty const& scalar) {
      if constexpr (std::is_same_v<oth_ty, ty>) { for (auto& val : *this) { val /= scalar; } }
      else if constexpr (std::is_same_v<decltype(std::declval<ty>() * scalar), ty>) { for (auto& val : *this) { val /= scalar; } }
      else { for (auto& val : *this) { val = static_cast<ty>(val / scalar); } }
      return *this;
      }


   // Helper for axis
   /*
   constexpr MyPoint<ty, SIZE, kind> Null_Point() const {
      return MyPoint<ty, SIZE, kind> axis;
      }
   */
   

   constexpr MyPoint<ty, SIZE, kind> Null_Point() const {
      MyPoint<ty, SIZE, kind> axis;
      return axis;
      }

   constexpr MyPoint<ty, SIZE, kind> X_Axis() const {
      MyPoint<ty, SIZE, kind> axis;
      axis.X(MyDistance<ty, kind>(1));
      return axis;
      }

   constexpr MyPoint<ty, SIZE, kind> Y_Axis() const {
      MyPoint<ty, SIZE, kind> axis;
      axis.Y(MyDistance<ty, kind>(1));
      return axis;
      }

   template <int S = SIZE, typename std::enable_if<(S == 3)>::type* = nullptr>
   constexpr MyPoint<ty, SIZE, kind> Z_Axis() const {
      MyPoint<ty, SIZE, kind> axis;
      axis.Z(MyDistance<ty, kind>(1));
      return axis;
      }


private:
   static const inline MyDistance<ty, kind> _V1 = MyDistance<ty, kind> { 1 };
   static const inline MyDistance<ty, kind> _V0 = MyDistance<ty, kind>{ 1 };
   static const inline  std::array<MyDistance<ty, kind>, 9> NormValues = { _V1, _V0, _V0, _V0, _V1, _V0, _V0, _V0, _V1 };

   //static const inline MyPoint<ty, SIZE, kind> v1 = 

   template <int POS, my_param_distance oth_ty = ty, int ASIZE = SIZE, MyDistanceKind oth_knd = kind>
   static constexpr MyPoint Initialize() {
      MyPoint<oth_ty, ASIZE, oth_knd> ret;
      if constexpr (ASIZE == 2) {
         if constexpr (POS == 0)      ret = { 1, 0 };
         else if constexpr (POS == 1) ret = { 0, 1 };
         }
      else if constexpr (ASIZE == 3) {
         if constexpr (POS == 0)      ret = { 1, 0, 0 };
         else if constexpr (POS == 1) ret = { 0, 1, 0 };
         else if constexpr (POS == 2) ret = { 0, 0, 1 };
         }
      return ret;
      }
   /*
   static inline MyPoint data1 = Initialize<0>();
   static inline MyPoint data2 = Initialize<1>();
   */
   };

 template <my_param_distance ty, int SIZE, MyDistanceKind kind>
 struct std::formatter<MyPoint<ty, SIZE, kind>> : std::formatter<std::string_view> {
      std::string format_string;
            constexpr auto parse(std::format_parse_context& ctx) {
         auto pos = ctx.begin();
         format_string = "{:";
         while (pos != ctx.end() && *pos != '}') {
            format_string += *pos;
            ++pos;
         }
         format_string += "}";
         return pos; // returns the iterator to the last parsed character in the format string, in this case we just swallow everything
        }

      auto format(MyPoint<ty, SIZE, kind> const& val, std::format_context& ctx) {
         std::string temp;
         
         std::string fmt = "(" + format_string;
         std::vformat_to(std::back_inserter(temp), fmt, std::make_format_args(val[0]));
         fmt = ", " + format_string;
         for (size_t idx = 1; idx < SIZE; ++idx) std::vformat_to(std::back_inserter(temp), fmt, std::make_format_args(val[idx]));
         temp += ")";
         return std::formatter<std::string_view>::format(temp, ctx);
      }
   };



template <std::floating_point ty>
using MyPoint2D = MyPoint<ty, 2>;

template <std::floating_point ty>
using MyPoint3D = MyPoint<ty, 3>;
