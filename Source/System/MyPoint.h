#pragma once
/*

Selektoren sind fertig


*/


#if __has_include("MyPoint.h")
   #include "MyAngle.h"
   #include "MyDistance.h"
#elif __has_include("System/MyPoint.h")
   #include "MyAngle.h"
   #include "MyDistance.h"
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
      out << "(";
      for (size_t i = 0; i < SIZE; ++i) out << (i > 0 ? ", " : "") << point[i];
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
   constexpr MyPoint(void) = default;

   // Kopierkonstruktor für MyPoint mit gleichen Ty
   MyPoint(const MyPoint<ty, SIZE, kind>& other) : std::array<MyDistance<ty, kind>, SIZE>(other) {}

   // Kopierkonstruktor für abweichenden Ty
   //template <typename oth_ty, typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty>>>

   template <MyDistanceKind oth_knd = kind, my_param_distance oth_ty = ty, 
      typename = std::enable_if_t<std::is_convertible_v<oth_ty, ty> && distance_kinds_convertible<kind, oth_knd>>>
   MyPoint(const MyPoint<oth_ty, SIZE, oth_knd>& other) {
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
      if (vec.size() != SIZE) { throw std::invalid_argument("Vector size does not match MyPoint size"); }
      if constexpr (oth_knd == kind && std::is_same_v<oth_ty, ty>) std::copy(vec.begin(), vec.end(), this->begin());
      else std::transform(vec.begin(), vec.end(), this->begin(), [](auto const & val) { return val.convert_to<kind, ty>(); });
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
      return MyDistance<ty, kind>(std::accumulate(products.begin(), products.end(), static_cast<ty>(0.0)));
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

   };


template <std::floating_point ty>
using MyPoint2D = MyPoint<ty, 2>;

template <std::floating_point ty>
using MyPoint3D = MyPoint<ty, 3>;
