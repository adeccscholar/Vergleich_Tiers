#pragma once

#include <iostream>

class MyNumbers {
friend std::ostream& operator << (std::ostream& out, MyNumbers const& data) {
           return out << data.flNumber;
           }

private:
   double flNumber = 0.0;
public:
   MyNumbers(void) = default;
   MyNumbers(MyNumbers const&) = default;
   MyNumbers(MyNumbers&&) = default;

   ~MyNumbers(void) = default;

   MyNumbers& operator = (MyNumbers const&) = default;
   MyNumbers& operator = (MyNumbers&&) = default;

   auto operator <=> (MyNumbers const&) const = default;


   [[nodistinct]] double const& Value() const { return flNumber;  }
   void Value(double const& val) { flNumber = val; }

   // 1. Ausbau
   MyNumbers(double const& val) : flNumber(val) { }
   operator double& () { return flNumber; }
   operator double const& () const& { return flNumber; }
   operator double () const { return flNumber; }



};
