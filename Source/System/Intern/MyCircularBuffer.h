#pragma once

#include <array>
#include <initializer_list>

#if __has_include("MyExceptions.h")
  #include "MyExceptions.h"
#elif __has_include("adecc_Tools/MyExceptions.h")
  #include "adecc_Tools/MyExceptions.h"
#endif

/// circular buffer instead the stack, method push ... initialize conctructor and view with iterator
template <typename ty, size_t SIZE>
class CircularBuffer : private std::array<ty, SIZE> {
   friend void swap(CircularBuffer& lhs, CircularBuffer& rhs) noexcept { lhs.swap(rhs); }
   private:
      size_t iStart = 0;
      size_t iSize  = 0;
   public:
      using std::array<ty, SIZE>::operator[];

      CircularBuffer() = default;

      CircularBuffer(CircularBuffer const& other) : iSize(other.iSize), iStart(other.iStart) {
         std::copy(other.begin(), other.end(), this->begin());
         }

      CircularBuffer(CircularBuffer&& other) noexcept { swap(other); }

      CircularBuffer(std::initializer_list<ty> items) : CircularBuffer() {
         for (ty const& value : items) push(value);
         }

      void copy(CircularBuffer const& other) {
         iSize  = other.iSize; 
         iStart = other.iStart;
         std::copy(other.begin(), other.end(), this->begin());
         }

      void swap(CircularBuffer& other) noexcept {
         using std::swap;
         std::swap(static_cast<std::array<ty, SIZE>&>(*this), static_cast<std::array<ty, SIZE>&>(other));
         std::swap(iStart, other.iStart);
         std::swap(iSize,  other.iSize);
         }

      void push(ty const& value) {
         (*this)[(iStart + iSize) % SIZE] = value;
         if (iSize == SIZE) iStart = (iStart + 1) % SIZE; 
         else ++iSize;
         }

      ty const& last_element() const {
         if (iSize > 0) return (*this)[(iStart + iSize - 1) % SIZE];
         else TMy_StandardError<std::out_of_range>("buffer is empty.");
         }

      void clear() {
         iSize = 0;
         iStart = 0;
         }

      bool empty() const {
         return iSize == 0;
         }

      class CircularBufferView {
         private:
            CircularBuffer const& buffer;
         public:
            CircularBufferView(CircularBuffer const& param) : buffer(param) {}
            CircularBufferView(CircularBufferView const& other) = default;

            class iterator {
                  private:
                     CircularBuffer const& buffer;
                     size_t          index;
                  public:
                     iterator(CircularBuffer const& param, size_t idx) : buffer(param), index(idx) {}
                     iterator(iterator const& other) = default;
                     ty const& operator*() { return buffer[index % SIZE]; }
                     ty const& operator*() const { return buffer[index % SIZE]; }
                     iterator& operator++() { ++index; return *this; }
                     //iterator& operator--() { ++index; return *this; }
                     bool operator!=(const iterator& other) const { return index != other.index;  }
                  };

            iterator begin() { return iterator(buffer, buffer.iStart); }
            iterator end() { return iterator(buffer, buffer.iStart + buffer.iSize); }

            iterator begin() const { return iterator(buffer, buffer.iStart); }
            iterator end() const { return iterator(buffer, buffer.iStart + buffer.iSize); }

         };

      CircularBufferView view() { return CircularBufferView(*this); }
      CircularBufferView view() const { return CircularBufferView(*this); }

   };


