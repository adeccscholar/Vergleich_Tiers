#pragma once

#include <vector>
#include <iterator>
#include <initializer_list>

enum MyBoundSafety : int { without, error, increase };

template <typename ty, class Allocator = std::allocator<ty>, MyBoundSafety level = MyBoundSafety::increase>
class MySafeVector : public std::vector<ty, Allocator> {
public:
   MySafeVector() = default;
   MySafeVector(MySafeVector const&) = default;
   MySafeVector(std::initializer_list<ty> initList) : std::vector<ty, Allocator>(initList) { }

   ty& operator [](std::size_t index) {
      if constexpr (level != MyBoundSafety::without) {
         if(index >= this->size()) {
            if constexpr (level == MyBoundSafety::error) {
               throw std::range_error("Index out of bound");
               }
            else if constexpr (level == MyBoundSafety::increase) {
               this->resize(index + 1, ty{ });
               }
            /*
            else {
               static_assert(always_false_v<ty>, "unexpected level for Bound Safety")
               }
            */
            }
         }
      return std::vector<ty>::operator[](index);
      }
   
   ty const& operator [](std::size_t index) const {
      if (index >= this->size()) {
         throw std::range_error("Index out of bound");
         }
      return std::vector<ty>::operator[](index);
      }



   };