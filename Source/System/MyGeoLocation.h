#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <type_traits>
#include <utility>
#include <tuple>
#include <cmath>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <optional>
#include <numbers>
//#include <charconv>
#include <ranges>

#if __has_include("MyAngle.h")
  #include "MyAngle.h" 
  #include "MyDistance.h"
  #include "MyPoint.h"
#elif __has_include("System/MyAngle.h")
  #include "System/MyAngle.h" 
  #include "System/MyDistance.h"
  #include "System/MyPoint.h"
#endif

using namespace std::string_view_literals;

enum class MyGeodeticRefType : int { WGS84, ETRS89, MercatorETRS89 };


template <std::floating_point ty>
class MyWGS84 : std::pair<MyAngle<ty, MyAngleKind::degree>, MyAngle<ty, MyAngleKind::degree>> {
   
public:
   using arc_ty = MyAngle<ty, MyAngleKind::degree>;

   MyWGS84(void) = default;
   MyWGS84(MyWGS84 const&) = default;
   MyWGS84(MyWGS84&&) noexcept = default;
   //MyWGS84(ty const& lon, ty const& lat) : std::pair<arc_ty, arc_ty>(lon, lat) { }
   MyWGS84(arc_ty const& lon, arc_ty const& lat) : std::pair<arc_ty, arc_ty>(lon, lat) { }
   MyWGS84(arc_ty && lon, arc_ty && lat) : std::pair<arc_ty, arc_ty>(lon, lat) { }

   virtual ~MyWGS84(void) = default;

   MyWGS84& operator = (MyWGS84 const&) = default;
   MyWGS84& operator = (MyWGS84&&) noexcept = default;

   auto operator <=> (MyWGS84 const&) const = default;

   void Longitude(arc_ty const& val) { std::get<0>(*this) = val; }
   void Latitude(arc_ty const& val) { std::get<1>(*this) = val; }

   arc_ty const& Longitude(void) const { return std::get<0>(*this); }
   arc_ty const& Latitude(void) const { return std::get<1>(*this); }
};

template <std::floating_point ty>
class MyETRS89 : std::tuple<char, int, ty, ty> {
public:
   MyETRS89(void) = default;
   MyETRS89(MyETRS89 const&) = default;
   MyETRS89(MyETRS89&&) noexcept = default;
   MyETRS89(char l, int z, ty const& east, ty const& north) : std::tuple<char, int, ty, ty>(l, z, east, north) { }

   virtual ~MyETRS89(void) = default;

   MyETRS89& operator = (MyETRS89 const&) = default;
   MyETRS89& operator = (MyETRS89&&) noexcept = default;

   auto operator <=> (MyETRS89 const&) const = default;

   void Letter(char val) const { std::get<0>(*this) = val; }
   void Zone(int val) const { std::get<1>(*this) = val; }
   void Easting(ty const& val) { std::get<2>(*this) = val; }
   void Northing(ty const& val) { std::get<3>(*this) = val; }

   char      Letter(void) const { return std::get<0>(*this); }
   int       Zone(void) const { return std::get<1>(*this); }
   ty const& Easting(void) const { return std::get<2>(*this); }
   ty const& Northing(void) const { return std::get<3>(*this); }

};


template <std::floating_point ty>
class MyMercatorETRS89 : public MyPoint<ty> {// std::pair<ty, ty> {
public:
   constexpr MyMercatorETRS89(void) = default;
   MyMercatorETRS89(MyMercatorETRS89 const&) = default;
   MyMercatorETRS89(MyMercatorETRS89&&) noexcept = default;
   MyMercatorETRS89(ty const& x, ty const& y) : MyPoint<ty>(x, y) { } //std::pair<ty, ty>(x, y) { }

   virtual ~MyMercatorETRS89(void) = default;

   MyMercatorETRS89& operator = (MyMercatorETRS89 const&) = default;
   MyMercatorETRS89& operator = (MyMercatorETRS89&&) noexcept = default;

   auto operator <=> (MyMercatorETRS89 const&) const = default;

};


template <std::floating_point ty = double, MyGeodeticRefType _kind = MyGeodeticRefType::WGS84>
using GeoBase = std::conditional_t<(_kind == MyGeodeticRefType::WGS84), MyWGS84<ty>,
   std::conditional_t<(_kind == MyGeodeticRefType::ETRS89), MyETRS89<ty>,
   std::conditional_t<(_kind == MyGeodeticRefType::MercatorETRS89), MyMercatorETRS89<ty>, void>>>;

template <typename T>
struct is_floating_point : std::is_floating_point<T> {};

template <std::floating_point ty = double, MyGeodeticRefType _kind = MyGeodeticRefType::WGS84>
class MyGeoLocation : public GeoBase<ty, _kind> {
public:
   MyGeoLocation(void) = default;
   MyGeoLocation(MyGeoLocation&&) noexcept = default;

  /*
   template <typename... Args,
      typename = std::enable_if_t<_kind == MyGeodeticRefType::WGS84 &&
                                  sizeof...(Args) == 2 &&
                                  std::conjunction_v<std::is_convertible<MyAngle<ty, MyAngleKind::degree>, Args>...> &&
                                  std::conjunction_v<std::is_lvalue_reference<Args>...>) >
      MyGeoLocation(Args... args) : GeoBase<ty, _kind>(std::forward<Args>(args)...) { }
  */
   template <std::floating_point oth_ty = ty, MyGeodeticRefType oth_knd = _kind,
      typename = std::enable_if_t<oth_knd == MyGeodeticRefType::WGS84 && std::is_same_v<oth_ty, ty>>>
   MyGeoLocation(typename MyWGS84<oth_ty>::arc_ty const& lon, typename MyWGS84<oth_ty>::arc_ty const& lat)
      : GeoBase<oth_ty, oth_knd>(lon, lat) {}

   /*
   template <typename Arg1, typename Arg2, typename... Rest,
      typename = std::enable_if_t<std::is_same_v<Arg1, char>&& std::is_same_v<Arg2, int> &&
      (_kind == MyGeodeticRefType::ETRS89) && sizeof...(Rest) == 2 &&
      (std::conjunction_v<std::is_floating_point<Rest>...>), void>>
      MyGeoLocation(Arg1 arg1, Arg2 arg2, Rest... rest) : GeoBase<ty, _kind>(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Rest>(rest)...) { }
   */
 

   template <std::floating_point other_ty, MyGeodeticRefType other_kind>
   MyGeoLocation(MyGeoLocation<other_ty, other_kind> const& other) {
      *this = convert_from(other);
   }
   

   MyGeoLocation& operator = (MyGeoLocation&&) noexcept = default;

   template <std::floating_point other_ty, MyGeodeticRefType other_kind>
   MyGeoLocation& operator = (const MyGeoLocation<other_ty, other_kind>& other) {
      *this = convert_from(other);
      return *this;
   }


   template <std::floating_point oth_ty, MyGeodeticRefType oth_knd>
   operator std::conditional_t<std::is_same_v<ty, oth_ty>&& _kind == oth_knd, const MyGeoLocation&, MyGeoLocation<oth_ty, oth_knd>>() const {
      if constexpr (std::is_same_v<ty, oth_ty> && _kind == oth_knd) {
         return *this;
      }
      else {
         return convert_to<oth_ty, oth_knd>(*this);
      }
   }


   double Test(MyGeoLocation<ty, MyGeodeticRefType::WGS84> const& other) {
      return 1.0;
   }

   template <std::floating_point oth_ty, MyGeodeticRefType oth_knd>
   auto convert_from(const MyGeoLocation<oth_ty, oth_knd>& other)
      -> std::conditional_t<std::is_same_v<ty, oth_ty>&& _kind == oth_knd, MyGeoLocation const&, MyGeoLocation> {
      if constexpr (std::is_same_v<ty, oth_ty> && _kind == oth_knd) { return static_cast<MyGeoLocation const&>(other); }
      else {
         return MyGeoLocation<ty, _kind>();
      }
      /*
      if constexpr (std::is_same_v<ty, double> && std::is_same_v<other_ty, float>) {
         // Konvertierung von float auf double
         return MyGeoLocation(static_cast<ty>(other.Longitude()), static_cast<ty>(other.Latitude()));
         }
      else if constexpr (std::is_same_v<ty, float> && std::is_same_v<other_ty, double>) {
         // Konvertierung von double auf float
         return MyGeoLocation(static_cast<ty>(other.Longitude()), static_cast<ty>(other.Latitude()));
         }
      else {
         // Fehlerfall oder nicht unterstützte Konvertierung
         // Hier können Sie entsprechende Maßnahmen ergreifen, z. B. eine Ausnahme werfen.
         throw std::runtime_error("Ungültige Konvertierung");
         }
      */
   }

   template <std::floating_point other_ty, MyGeodeticRefType other_kind>
   MyGeoLocation<other_ty, other_kind> convert_to() const {
      /*
      if constexpr (std::is_same_v<ty, double> && std::is_same_v<other_ty, float>) {
         // Konvertierung von double auf float
         return MyGeoLocation<other_ty, other_kind>(static_cast<other_ty>(this->Longitude()), static_cast<other_ty>(this->Latitude()));
      }
      else if constexpr (std::is_same_v<ty, float> && std::is_same_v<other_ty, double>) {
         // Konvertierung von float auf double
         return MyGeoLocation<other_ty, other_kind>(static_cast<other_ty>(this->Longitude()), static_cast<other_ty>(this->Latitude()));
      }
      else {
         // Fehlerfall oder nicht unterstützte Konvertierung
         // Hier können Sie entsprechende Maßnahmen ergreifen, z. B. eine Ausnahme werfen.
         throw std::runtime_error("Ungültige Konvertierung");
      }
      */
      return MyGeoLocation<other_ty, other_kind>();
   }
};

