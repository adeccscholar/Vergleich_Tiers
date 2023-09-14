#pragma once

#if __has_include("MyPoint.h")
   #include "MyAngle.h"
   #include "MyDistance.h"
#elif __has_include("System/MyPoint.h")
   #include "MyAngle.h"
   #include "MyDistance.h"
#endif

#include <iostream>
#include <sstream>
#include <utility>
#include <stdexcept>
#include <cmath>
#include <string>
#include <numbers>
#include <type_traits>
#include <format>



template <std::floating_point ty>
class MyPoint : public std::pair<ty, ty> {
   friend std::ostream& operator << (std::ostream& out, MyPoint<ty> const& point) {
      return out << "(" << point.first << ", " << point.second << ")";
      }

   friend std::istream& operator >> (std::istream& in, MyPoint<ty>& point) {
      char c;
      in >> c >> point.first >> c >> point.second >> c;
      return in;
      }

   friend MyPoint<ty> operator + (MyPoint<ty> const& a, MyPoint<ty> const& b) {
      return MyPoint<ty>(a) += b;
      }

   friend MyPoint<ty> operator + (MyPoint const& a, ty const& b) {
      return MyPoint<ty>(a) += b;
      }

   friend MyPoint<ty> operator - (MyPoint<ty> const& a, MyPoint<ty> const& b) {
      return MyPoint<ty>(a.first - b.first, a.second - b.second);
      }

   friend MyPoint<ty> operator - (MyPoint<ty> const& a, ty const& b) {
      return MyPoint<ty>(a.first - b, a.second);
      }

   friend MyPoint<ty> operator * (MyPoint const& a, ty const& b) {
      return MyPoint<ty>(a.first * b, a.second * b);
      }

   friend MyPoint<ty> operator / (MyPoint const& a, ty const& b) {
      return MyPoint<ty>(a.first / b, a.second / b);
      }


public:
   constexpr MyPoint(void) : MyPoint<ty>(ty(), ty()) { }
   constexpr MyPoint(ty const& x, ty const& y = ty()) : std::pair<ty, ty>(x, y) { }
   constexpr MyPoint(ty const& distance, MyAngle<ty> const& angle) : MyPoint<ty>() { 
      DistanceAndAngle(distance, angle); 
      }

   MyPoint(std::string const& text) {
      std::istringstream ss(text);
      ss >> *this;
      if (!ss) throw std::invalid_argument("ungültiges Koordinatenpaar");
      }

   constexpr MyPoint(MyPoint const& other) noexcept = default;
   constexpr MyPoint(MyPoint&& other) noexcept = default;

   MyPoint& operator = (MyPoint const&) = default;
   MyPoint& operator = (MyPoint&&) noexcept = default;
   virtual ~MyPoint() = default;

   MyPoint& operator += (MyPoint const& other) {
      this->first += other.first;
      this->second += other.second;
      return *this;
      }

   MyPoint& operator += (ty const& other) {
      this->first += other;
      return *this;
      }

   MyPoint& operator -= (MyPoint const& other) {
      this->first -= other.first;
      this->second -= other.second;
      return *this;
      }

   MyPoint& operator -= (ty const& other) {
      this->first -= other;
      return *this;
      }

   MyPoint& operator *= (ty const& other) {
      this->first  *= other;
      this->second *= other;
      return *this;
      }

   MyPoint& operator /= (ty const& other) {
      this->first  /= other;
      this->second /= other;
      return *this;
      }

   operator std::string() const { return std::format("({}, {})", this->first, this->second); }

   auto operator <=> (MyPoint const& other) const = default;


   ty const& X(void) const { return this->first; }
   ty const& Y(void) const { return this->second; }
   ty Distance(void) const { return distance(*this, MyPoint<ty>()); }
   MyAngle<ty> Angle(void) const { return angle(MyPoint<ty>(), *this); }

   constexpr void X(ty const& newVal) { this->first = newVal; }
   constexpr void Y(ty const& newVal) { this->second = newVal; }

   constexpr void DistanceAndAngle(ty const& distance, MyAngle<ty> const& angleInDegrees) {
      const ty angleInRadians = angleInDegrees * (std::numbers::pi_v<ty> / 180.0); 
      X(X() + distance * std::cos(angleInRadians));
      Y(Y() + distance * std::sin(angleInRadians));
      }

   ty distance(MyPoint const& other) const { return distance(*this, other); }

   MyAngle<ty> angle(void) const {
      return std::atan2(this->second, this->first) * (180.0 / std::numbers::pi_v<ty>);
      }

   MyAngle<ty> angle(MyPoint const& other) const {  return angle(*this, other); }

   auto constexpr addDistanceAndAngle(ty const& distance, MyAngle<ty> const& angleInDegrees) const {
      const ty angleInRadians = angleInDegrees * (std::numbers::pi_v<ty> / 180.0); // Umrechnung in Bogenmaß
      return MyPoint<ty>(X() + distance * std::cos(angleInRadians), Y() + distance * std::sin(angleInRadians));
      }


   static ty distance(MyPoint const& a, MyPoint const& b) {
      ty dx = a.first - b.first;
      ty dy = a.second - b.second;
      return std::sqrt(dx * dx + dy * dy);
      }

   static MyAngle<ty> angle(MyPoint const& a, MyPoint const& b) {
      return (b - a).angle();
      }

   static ty manhattan_distance(MyPoint const& a, MyPoint const& b) {
      return std::fabs(a.first - b.first) + std::fabs(a.second - b.second);
      }

   };
