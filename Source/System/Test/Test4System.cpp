#include "Test4System.h"
#include <System/MyNumber.h>
#include <System/MyDistance.h>
#include <System/MyGeoLocation.h>

#include <System/MySafeVector.h>
#include <System/MySafeNumber.h>

#include "MyTestSuite.h"

#include <cmath>
#include <iomanip>
#include <sstream>

#include <chrono>
#include <random>
#include <functional>
#include <algorithm>
#include <ranges>

#include <limits>
#include <cfenv>

namespace test_space {

 // --------------------------------------------------------------
 // method to call and test instances of the class MySafeVector
 // --------------------------------------------------------------
void FillVector(MySafeVector<int>& result, size_t count) {
      std::mt19937 rand_value;
      std::uniform_int_distribution<int> distri_function(1, 100);
      rand_value.seed(std::chrono::system_clock::now().time_since_epoch().count());
      std::function<int()> rand_func = bind(distri_function, rand_value);

      result.resize(count);
      std::generate(result.begin(), result.end(), [rand_func]() { return rand_func(); });
   }

 void Test4SafeVector(std::ostream& out) {
    using namespace std::string_literals;
    
    using vecStrings = MySafeVector<std::string>;
    using tplType = std::tuple<vecStrings, vecStrings, bool>;

    tplType tplist = tplType { vecStrings { "key1"s, "key2"s }, vecStrings { "field1"s, "field2"s, "field3"s }, true };

    for(auto const& tmp : std::get<0>(tplist)) {
       out << tmp << "\n";
       }
 
    MySafeVector<int> values;
    FillVector(values, 20);
    std::sort(values.begin(), values.end());
    for (auto& val : values) out << val << "\n";
    }

 // --------------------------------------------------------------
 // method to call and test instances of the class MyNumber
 // --------------------------------------------------------------
 double zero_val = 0.0;

 void TestFPEnv(int exceptions, std::ostream& out) {
    if (exceptions & FE_INEXACT)   out << "FE_INEXACT was triggered: Inexact result detected." << std::endl;
    if (exceptions & FE_UNDERFLOW) out << "FE_UNDERFLOW was triggered: Underflow detected." << std::endl;
    if (exceptions & FE_OVERFLOW)  out << "FE_OVERFLOW was triggered: Overflow detected." << std::endl;
    if (exceptions & FE_DIVBYZERO) out << "FE_DIVBYZERO was triggered: Division by zero detected." << std::endl;
    if (exceptions & FE_INVALID)   out << "FE_INVALID was triggered: Invalid operation detected." << std::endl;
    }


 
void Test4Path(std::ostream& out) {
   std::vector<std::string> vecInputPath = { "D:\\Projekte\\GitHub\\Vergleich_Schichten\\Source\\System\\MySafeNumber.h",
                                             "D:\\System\\MySafeNumber.h",
                                             "D:\\MySafeNumber.h",
                                             "MySafeNumber.h",
                                             "D:"s,
                                             "D:\\"s,
                                             "/"s,
                                             ""s };
   for (auto const& inputPath : vecInputPath) {
      out << "Ursprünglicher Pfad: " << inputPath << std::endl;
      out << "Vereinfachter Pfad: " << TMyExceptionInformation::cutPath(inputPath) << std::endl;
      }


   TMyExceptionInformation test_info;
   out << test_info.FilePosition() << "\n";
   out << TMyExceptionInformation::FilePosition(src_loc::current()) << "\n";
   }

 void Test4Numbers(std::ostream& out) {
    static_assert(MySafety::is_my_safe_number_type<MySafety::TNumber<int, 10>>, "TNumber concept don't work.");

    TestSuite tests("check implementations of MySafety::TNumber", out);

    tests.Check_fn_exception<std::runtime_error>("test to construct a int number with long long value and strict types", [&out]() {
       MySafety::TNumber<int, MySafety::combineNumberSafety(MySafety::ENumberSafety::withException,
                                                            MySafety::ENumberSafety::withStrictTypes,
                                                            //MySafety::ENumberSafety::withPosition,
                                                            MySafety::ENumberSafety::withAdditionalData,
                                                            MySafety::ENumberSafety::withOptionalChecks)> snll{ 0ll };
       });

    tests.Check_fn_exception<std::runtime_error>("assignment of negative value to a unsigned Number: ", [&out]() {
               MySafety::TNumber<uint32_t> su;
               su = -1;
               out << su << "\n";
               });

    tests.Check_fn_exception<std::runtime_error>("assignment of negative value with declaration to a unsigned Number: ", [&out]() {
       MySafety::TNumber<uint32_t> su = -1;
       out << su << "\n";
       });

    
    tests.Check_fn("test compare with another number type.", [&out] {

    size_t iCnt = 0, iErr = 0;
    static auto CompFunc = [&out, &iCnt, &iErr](auto const& val1, auto const& val2, auto result) {
       auto comp = val1 <=> val2;
       ++iCnt;
       out << "Value 1 of type " << typeid(decltype(val1)).name() << ": " << val1 << " <=> "
          << "Value 2 of type " << typeid(decltype(val2)).name() << ": " << val2 << " = ";

       if constexpr (std::is_same_v<decltype(comp), std::partial_ordering>) {
          if (comp == std::partial_ordering::less)            out << "partial less";
          else if (comp == std::partial_ordering::equivalent) out << "partial equivalent";
          else if (comp == std::partial_ordering::greater)    out << "partial greater";
          else if (comp == std::partial_ordering::unordered)  out << "partial unordered";
          else                                               out << "partial ordering unexpected value";
          }
       else if constexpr (std::is_same_v<decltype(comp), std::strong_ordering>) {
          if (comp == std::strong_ordering::less)            out << "strong less";
          else if (comp == std::strong_ordering::equal)      out << "strong equal";
          else if (comp == std::strong_ordering::greater)    out << "strong greater";
          else                                               out << "strong ordering unexpected value";
          }
       out << "  (" << typeid(comp).name() << "). The Result is " << (comp == result ? "correct." : "wrong.") << std::endl;
       if (comp != result) ++iErr;
       return comp == result;
       };

       MySafety::TNumber<int>   sn1{ 25 };
       MySafety::TNumber<short> sn2;
       MySafety::TNumber<short> sn3 { 40 };
       MySafety::TNumber<short, MySafety::SilentSafety> sn4{ std::numeric_limits<long long>::max() };
       MySafety::TNumber<long long> sn5 { std::numeric_limits<long long>::max() };

       MySafety::TNumber<long long> sn6;
       sn6 = sn1;
       sn2.reset();
       out << std::endl;
       CompFunc(sn1, sn2, std::partial_ordering::unordered);
       CompFunc(sn1, sn3, std::partial_ordering::less);
       CompFunc(sn1, sn4, std::partial_ordering::unordered);
       CompFunc(sn1, sn5, std::partial_ordering::less);
       CompFunc(sn1, sn6, std::partial_ordering::equivalent);
       CompFunc(sn1, 30,  std::partial_ordering::less);
       CompFunc(sn1, 15,  std::partial_ordering::greater);
       CompFunc(sn1, MySafety::TValue { 25 }, std::partial_ordering::equivalent);
       CompFunc(sn1, MySafety::TValue{ std::numeric_limits<unsigned int>::max() }, std::partial_ordering::less);
       CompFunc(sn1, MySafety::TValue{ std::numeric_limits<long long>::max() }, std::partial_ordering::less);

       if (iErr > 0) {
          std::ostringstream os;
          os << iCnt << " tests to compare TNumber finished with " << iErr << " errors\n";
          throw TMy_RuntimeError(os.str());
          }
       
       });

    tests.Check_fn("test relational operators with standard number type.", [&out] {
       static std::array<std::string, 6> strOps = { "=="s, "!="s, "< "s, "<="s, "> "s, ">="s };
       size_t iCnt = 0, iErr = 0;
       static auto CompFunc = [&out, &iCnt, &iErr](auto const& val1, auto const& val2, std::array<bool, 6> results) {
          bool test_result;
          for(size_t i = 0; auto result : results) {
             ++iCnt;
             out << val1 << " " << strOps[i] << " " << val2;
             switch(i) {
                case 0: test_result = val1 == val2; break;
                case 1: test_result = val1 != val2; break;
                case 2: test_result = val1 <  val2; break;
                case 3: test_result = val1 <= val2; break;
                case 4: test_result = val1 >  val2; break;
                case 5: test_result = val1 >= val2; break;
                default: throw TMy_RuntimeError("unexpected problem inside of the test for relational operators");
                }
             out << " The Result is " << (test_result ? "true" : "false") 
                 << ", expected was " << (result ? "true" : "false") << ". Test is ";
             if(test_result == result) {
                out << "successful.";
                }
             else {
                out << "failed.";
                ++iErr;
                }
             out << std::endl;
             ++i;
             }
          return iErr;
          };


       MySafety::TNumber<int>          sn1 { 25 };
       MySafety::TNumber<unsigned int> sn2 { 25 };
       MySafety::TNumber<long long>    sn3 { 26 };
       MySafety::TNumber<int>          sn4 {  0 };
       MySafety::TNumber<int>          sn5, sn6;
       MySafety::TNumber<int, MySafety::SilentSafety> sn7 { std::numeric_limits<long long>::min() };
       MySafety::TNumber<int, MySafety::SilentSafety> sn8 { std::numeric_limits<long long>::max() };

       MySafety::TNumber<int,            MySafety::StandardSafety> ssn1{ 25 };
       MySafety::TNumber<short,          MySafety::StandardSafety> ssn2{ 5 };
       MySafety::TNumber<unsigned short, MySafety::StandardSafety> ssn3{ 25 };
       MySafety::TNumber<long long,      MySafety::StandardSafety> ssn4{ std::numeric_limits<long long>::max() };
       MySafety::TNumber<int,            MySafety::StandardSafety> ssn5{  };
       //MySafety::TNumber<int,            MySafety::StandardSafety> ssn6{ std::numeric_limits<long long>::max() };

       out << "\ncompare with partial_ordering:\n";
       CompFunc(sn1, sn2, { true,  false, false, true,  false, true  });
       CompFunc(sn1, sn3, { false, true,  true,  true,  false, false });
       CompFunc(sn1, sn4, { false, true,  false, false, true,  true  });
       CompFunc(sn1, sn5, { false, true,  false, false, false, false });
       CompFunc(sn5, sn6, { true,  false, false, true,  false, true  });
       CompFunc(sn1, sn7, { false, true,  false, false, false, false });
       CompFunc(sn5, sn7, { false, true,  false, false, false, false });
       CompFunc(sn7, sn8, { false, true,  false, false, false, false });
       out << "\ncompare with strong_ordering:\n";
       CompFunc(ssn1, ssn2, { false, true,  false, false, true,  true  });
       CompFunc(ssn1, ssn3, { true,  false, false, true,  false, true  });
       CompFunc(ssn1, ssn4, { false, true,  true,  true,  false, false });
       CompFunc(ssn1, ssn5, { false, true,  false, false, true,  true  });
       //CompFunc(ssn1, ssn6, { false, true,  false, false, true,  true  });


       if (iErr > 0) {
          std::ostringstream os;
          os << iCnt << " tests to compare TNumber finished with " << iErr << " errors\n";
          throw TMy_RuntimeError(os.str());
          }

       });

    tests.Check_fn("test compare with standard number type.", [&out] {
       size_t iCnt = 0, iErr = 0;
       static auto CompFunc = [&out, &iCnt, &iErr](auto const& val1, auto const& val2, auto result) {
          auto comp = val1 <=> val2;
          ++iCnt;
          out << "Value 1 of type " << typeid(decltype(val1)).name() << ": " << val1 << " <=> "
              << "Value 2 of type " << typeid(decltype(val2)).name() << ": " << val2 << " = ";

          if constexpr (std::is_same_v<decltype(comp), std::partial_ordering>) {
             if (comp == std::partial_ordering::less)            out << "partial less";
             else if (comp == std::partial_ordering::equivalent) out << "partial equivalent";
             else if (comp == std::partial_ordering::greater)    out << "partial greater";
             else if (comp == std::partial_ordering::unordered)  out << "partial unordered";
             else                                               out << "partial ordering unexpected value";
          }
          else if constexpr (std::is_same_v<decltype(comp), std::strong_ordering>) {
             if (comp == std::strong_ordering::less)            out << "strong less";
             else if (comp == std::strong_ordering::equal)      out << "strong equal";
             else if (comp == std::strong_ordering::greater)    out << "strong greater";
             else                                               out << "strong ordering unexpected value";
          }
          out << "  (" << typeid(comp).name() << "). The Result is " << (comp == result ? "correct." : "wrong.") << std::endl;
          if (comp != result) ++iErr;
          return comp == result;
          };


       MySafety::TNumber<int, MySafety::StandardSafety>   sn1{ 25 };
       MySafety::TNumber<short, MySafety::StandardSafety> sn2{ 5  };

       MySafety::TNumber<unsigned short, MySafety::StandardSafety> sn3{ 5 };
       MySafety::TNumber<long long, MySafety::StandardSafety> sn4{ std::numeric_limits<long long>::min() };

       out << std::endl;
       /*
       using std::tie;
       using std::make_tuple;

       tie(iCnt,iErr) = CompFunc(sn1,sn2, std::strong_ordering::greater)  ? make_tuple(++iCnt,iErr) : make_tuple(++iCnt,++iErr);
       tie(iCnt,iErr) = CompFunc(sn1, 30, std::strong_ordering::less)     ? make_tuple(++iCnt,iErr) : make_tuple(++iCnt,++iErr);

       tie(iCnt,iErr) = CompFunc(sn3, sn4, std::strong_ordering::greater) ? make_tuple(++iCnt,iErr) : make_tuple(++iCnt,++iErr);
       sn4 = std::numeric_limits<long long>::max();
       tie(iCnt,iErr) = CompFunc(sn3, sn4, std::strong_ordering::less)    ? make_tuple(++iCnt,iErr) : make_tuple(++iCnt,++iErr);
       sn4 = sn3;
       tie(iCnt,iErr) = CompFunc(sn3, sn4, std::strong_ordering::equal)   ? make_tuple(++iCnt,iErr) : make_tuple(++iCnt,++iErr);
       */  
       CompFunc(sn1, sn2, std::strong_ordering::greater);
       CompFunc(sn1, 30, std::strong_ordering::less);

       CompFunc(sn3, sn4, std::strong_ordering::greater);
       sn4 = std::numeric_limits<long long>::max();
       CompFunc(sn3, sn4, std::strong_ordering::less);
       sn4 = sn3;
       CompFunc(sn3, sn4, std::strong_ordering::equal);

       if(iErr > 0) {
          std::ostringstream os;
          os << iCnt << " tests to compare TNumber finished with " << iErr << " errors\n";
          throw std::runtime_error(os.str());
          }

       });

    tests.Check_fn("Test Move to show the copy ", [&out] {
               int value =  2 ;
               int const& ref = value;

               int test = std::move(ref);
               out << "Test:  "  << test << '\n';
               out << "Value: " << value << '\n';

               });

    tests.Check_fn("test assignment with a TValue auf the same type", [&out]() {
               MySafety::TNumber<int> sn{ 25 };
               MySafety::TValue<int, false> tttt(30);
               sn = tttt;
               out << "\nvalue = " << sn << "\n";
               });


    tests.Check_fn_exception<std::runtime_error>("test type dedection with guard and assignment with negative value", [&out]() {
       auto test = MySafety::TNumber { 1u };
       test = -3;
       out << "\nvalue = " << test << "\n";
       });

    tests.Check_fn("test empty number", [&out]() {
               MySafety::TNumber<int> e;
               out << "\nvalue = " << e << "\n";
               });

    tests.Check_fn_exception("test to construct a int number with a to small long long value", [&out]() {
               MySafety::TNumber<int> snll { std::numeric_limits<long long>::min() };
               });

    tests.Check_fn_exception("test to assign a int with a to large unsigned int value", [&out]() {
               MySafety::TNumber<int> snll;
               //MySafety::TNumber<int> snll{ std::numeric_limits<uint32_t>::max() };
               //snll = std::numeric_limits<uint32_t>::max();
               snll = MySafety::TValue(std::numeric_limits<uint32_t>::max());
               });

    tests.Finish();
/*
    out << "Size of bool: " << sizeof(bool) << " bytes" << std::endl;
    out << "Size of int:  " << sizeof(int) << " bytes" << std::endl;
    out << "Size of optional<int>: " << sizeof(std::optional<int>) << " bytes" << std::endl;

    out << "\nTest MySafety::TNumber\n";
;
    MySafety::TNumber<int> sn { 25 };
    out << "Size of SafeNumber: " << sizeof(sn) << " bytes" << std::endl;
    MySafety::TNumber<int> nsn{ -25 };
    
    long long val = 1;
    MySafety::TNumber<int, MySafety::combineNumberSafety(MySafety::ENumberSafety::withException,
                                                         MySafety::ENumberSafety::withStrictTypes,
                                                         //MySafety::ENumberSafety::withPosition,
                                                         MySafety::ENumberSafety::withOptionalChecks)> sn_o{ val };
    try {
       out << (sn +  5) << "\n";
       out << (sn * 50) << "\n";
       out << (sn /  5) << "\n";
      // out << sn.pow(2) << "\n";
       out << nsn << "\n";
       out << nsn.abs() << "\n";
       out << ++sn_o << "\n";
       sn_o.reset();
       out << ++sn_o << "\n";

       }
    catch(std::exception& ex) {
       out << ex.what() << "\n";
       }

    out << "\nadditional test for preparation\n";
    int testabs = std::abs(std::numeric_limits<int>::min());
    out << testabs << " : " << std::numeric_limits<int>::min() << " ... " << std::numeric_limits<int>::max() << std::endl;
    */
    }

void Test4Positions(std::ostream& out) {
   MySafety::myPositions<int, 12> pos;
   int i = 0;
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   //*
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", i++, true, MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   //*/
   for (int i = 0;  auto const& val : pos.view()  ) {
      out << std::format("{:2d}: {}\n", ++i, val);
      }

   pos.clear();
   out << "leer\n";
   i = 0;
   pos.push({ "Testmeldung", i++, std::nullopt,  std::nullopt, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", 0, false,   MySafety::ENumberStatus::ok, src_loc::current(), std::chrono::system_clock::now() });
   pos.push({ "Testmeldung", 0, false,   MySafety::ENumberStatus::uninitialized, src_loc::current(), std::chrono::system_clock::now() });
   for (int i = 0; auto val : pos.view()) {
      out << std::format("{:2d}: {}\n", ++i, val);
      }
   }


// --------------------------------------------------------------
// method to call and test instances of the class MyDistance
// --------------------------------------------------------------
void Test4Distance(std::ostream& out) {
   using namespace my_distance_literals;
   out << "Method to test the MyDistance<kind, ty> class with different units\n";
   auto entfernung = 1000.0_meter;
   MyDistance<double, MyDistanceKind::kilometer> entfernung2{ entfernung };
   bool cc = (entfernung == entfernung2);
   //out << std::format(out.getloc(), "{:10.2Lf}\n", entfernung);
   out << std::format(out.getloc(), "{:10.1Lf}\n", entfernung);
   out << std::setprecision(2) << entfernung << "\n";
   out << entfernung << " " << (entfernung == entfernung2 ? "=" : "<>") << " " << entfernung2 << "\n";
   entfernung = entfernung + 500._meter;
   entfernung = entfernung * 2;

   out << entfernung << "  (in yards: " << entfernung.Distance<MyDistanceKind::yards>() << ")\n";

   MyDistance<double, MyDistanceKind::yards> test_yards = entfernung;
   out << entfernung << " = " << test_yards << "\n";

   out << entfernung << " + " << test_yards << " = " << entfernung + test_yards << "\n";

   out << entfernung * 2 << " = " << entfernung + test_yards << " = "
      << std::boolalpha << (entfernung * 2 == entfernung + test_yards) << "\n";

   try {
      double tt = 12;
      entfernung.Distance(tt);
      out << entfernung << "\n";
   } 
   catch(std::exception const& ex) {
      out << "\n\nError detected: " << ex.what() << "\n";
      }

   out << "\nTest passed.\n\n";
}

void Test4FPE(std::ostream& out) {
   long long constexpr value0 = std::numeric_limits<long long>::max();
   double constexpr value1 = static_cast<double>(std::numeric_limits<long long>::max());
   double constexpr value2 = static_cast<double>(std::numeric_limits<double>::max());

   std::feclearexcept(FE_ALL_EXCEPT);
   long long value3 = static_cast<long long>(std::numeric_limits<double>::max());
   TestFPEnv(std::fetestexcept(FE_ALL_EXCEPT), out);

   out << "\n\nMaximal long long: " << value0 << std::endl
      << "Maximal double as long long: " << value3 << std::endl
      << "Maximal long long as double: " << value1 << std::endl
      << "Maximal double: " << value2 << std::endl;

   MyNumbers test1, test2(3.0);
   out << "Method to test the MyNumber class\n";
   out << "test1 = " << test1 << "\n";
   out << "test2 = " << test2 << "\n";
   out << "test1 + 5 = " << test1 + 5 << "\n";
   test1 += 5;
   out << "Test1 + test2 = " << test1 + test2 << "\n";

   // "Overflow" für Gleitkommazahlen
   out << std::boolalpha << std::scientific;
   test1 = DBL_MAX;
   test2 = DBL_MIN;
   out << "test1 = " << test1 << " / " << std::isinf(test1) << "\n";
   out << "test2 = " << test2 << " / " << std::isinf(test1) << "\n";
   out << "epsilon = " << std::numeric_limits<double>::epsilon() << " / " << std::isnan(std::numeric_limits<double>::epsilon()) << "\n";
   test1 *= 2.0;
   test2 /= 2.0;
   out << "test1 = " << test1 << " / " << std::isinf(test1) << "\n";
   out << "test2 = " << test2 << " / " << std::isinf(test1) << "\n";

   out << "epsilon = " << std::numeric_limits<double>::epsilon() << "\n";
   out << "min     = " << std::numeric_limits<double>::min() << "\n";
   out << "max     = " << std::numeric_limits<double>::max() << "\n";

   out << "\n\nInf ? " << (float)std::numeric_limits<double>::max() << "\n";
   double from1 = 0, to1 = std::nextafter(from1, 1.);
   out << "The next representable double after " << std::setprecision(20) << from1
      << " is " << to1
      << std::hexfloat << " (" << to1 << ")\n" << std::defaultfloat;

   std::feclearexcept(FE_ALL_EXCEPT);
   out << "1.0/0.0 = " << 1.0 / zero_val << '\n';
   if (std::fetestexcept(FE_DIVBYZERO))
      out << "division by zero reported\n";
   else
      out << "division by zero not reported\n";

   std::feclearexcept(FE_ALL_EXCEPT);
   out << "1.0/10 = " << 1. / 10 << '\n';
   if (std::fetestexcept(FE_INEXACT))
      out << "inexact result reported\n";
   else
      out << "inexact result not reported\n";


   double result = std::numeric_limits<double>::max(); // Maximaler Wert für double
   result = result * 2.0; // Versuch eines Überlaufs

   // Prüfen, ob FE_OVERFLOW ausgelöst wurde
   if (std::fetestexcept(FE_OVERFLOW)) {
      out << "FE_OVERFLOW wurde ausgelöst: Überlauf erkannt." << std::endl;
      }
   else {
      out << "FE_OVERFLOW wurde nicht ausgelöst: Kein Überlauf erkannt." << std::endl;
      }

   double constexpr base = std::numeric_limits<double>::max(); // Maximaler Wert für double
   double constexpr exponent = 2.0; // Große Potenz

   result = std::pow(base, exponent); // Potenzberechnung

   // Prüfen, ob FE_OVERFLOW ausgelöst wurde
   if (std::fetestexcept(FE_OVERFLOW)) {
      out << "Pow FE_OVERFLOW wurde ausgelöst: Überlauf erkannt." << std::endl;
      }
   else {
      out << "Pow FE_OVERFLOW wurde nicht ausgelöst: Kein Überlauf erkannt." << std::endl;
      }


   std::feclearexcept(FE_ALL_EXCEPT);

   result = std::numeric_limits<double>::min(); // Kleiner als der kleinste darstellbare Wert für double
   result = result / 20000000000000000.0; // Versuch eines Unterlaufs

   // Prüfen, ob FE_UNDERFLOW ausgelöst wurde
   if (std::fetestexcept(FE_UNDERFLOW)) {
      out << "FE_UNDERFLOW wurde ausgelöst: Unterlauf erkannt." << std::endl;
      }
   else {
      out << "FE_UNDERFLOW wurde nicht ausgelöst: Kein Unterlauf erkannt." << std::endl;
      }
   /*
   std::feclearexcept(FE_ALL_EXCEPT);
   out << "pow = " <<  << '\n';
   if (std::fetestexcept(FE_INEXACT))
      out << "inexact result reported\n";
   else
      out << "inexact result not reported\n";
   */

   }


// --------------------------------------------------------------
// method to call and test instances of the class MyDistance
// --------------------------------------------------------------
void Test4Angle(std::ostream& out) {
   using namespace my_angle_literals;

   out << "Method to test the MyDistance<kind, ty> class with different units\n";
   // play with MyAngle

   MySexagesimalAngle<int> test_60_int("52° 31' 16\"");
   out << test_60_int << " in decimal = " << std::setprecision(6) << test_60_int.DegreesAsDecimal() << "\n";

   MySexagesimalAngle<int, double> test_60_float("52° 31' 16.11\"");
   out << test_60_float << " in decimal = " << std::setprecision(6) << test_60_float.DegreesAsDecimal() << "\n";

   MySexagesimalAngle<int, double> bln_tv_tower_lat("52° 31' 16.25\""s),
      bln_tv_tower_lon("13° 24' 40.50\"");  // ungefähre Position Berliner Fernsehturm
   MyGeoLocation<double, MyGeodeticRefType::WGS84> bln_tv_tower2(bln_tv_tower_lat, bln_tv_tower_lon);
   out << "berlin tv tower at " << bln_tv_tower2 << "\n";

   MyWGS84<double>  bln_tv_tower(52.1_deg, 15.2_deg);
   MyAngle<double, MyAngleKind::degree> t1(52.1_deg), t2(15.2_deg);


   auto test = 0.25_rad;
   MyAngle<double, MyAngleKind::degree> tx = test;
   test.Check();
   test *= 2;
   test += 10.0_deg;
   auto cmp = test.convert_to<MyAngleKind::degree>();
   out << std::format("{:9.6f} {} {:7.3f}\n", test, test == cmp ? " == " : " != ", cmp);

   bool check = (test == cmp);
   auto calc = test + cmp;

   MyAngle<double, MyAngleKind::degree> tmp{ 0 };
   std::string w = "13";
   tmp.Angle(w);
   /*
   out << std::format("Winkel in deg: {:10.2f}\n", tmp.Angle());
   out << std::format("Winkel in rad: {:10.6f}\n", tmp.Angle<MyAngleKind::radian>());
   out << std::format("Winkel in deg: {:10.2f}\n", tmp);
   out << std::format("Winkel in rad: {:10.6f}\n", tmp.convert_to<MyAngleKind::radian>());
   */
   auto test2 = 120.0_deg;
   out << std::setprecision(4);
   for (auto const& number : { 45._deg, 90._deg, 120._deg }) {
      out << "sin(" << number << ") = " << number.sin() << "\n";
      out << "cos(" << number << ") = " << number.cos() << "\n";
   }

   out << "\nTest passed.\n\n";
}

// --------------------------------------------------------------
//  test for MyPoint
// --------------------------------------------------------------

template <typename stream_ty, class... Args>
void myPrint(stream_ty& out, std::vector<std::string> const& fmts, Args&&... args) {
   auto fmt = std::accumulate(fmts.begin(), fmts.end(), std::string());
   std::vprint_nonunicode(out, fmt, std::make_format_args(args...));
   };

template <typename stream_ty, class... Args>
void myPrintLn(stream_ty& out, std::vector<std::string> const& fmts, Args&&... args) {
   auto fmt = std::accumulate(fmts.begin(), fmts.end(), std::string());
   fmt += "\n";
   std::vprint_nonunicode(out, fmt, std::make_format_args(args...));
   };

template <MyDistanceKind oth_knd, MyDistanceKind knd1, my_param_distance ty1, int SIZE1, MyDistanceKind knd2, my_param_distance ty2, int SIZE2>
   requires (SIZE1 == SIZE2)
void TestSequenze(std::ostream& out, std::string const& strText, MyPoint<ty1, SIZE1, knd1> const& p1, MyPoint<ty2, SIZE2, knd2> const& p2) {
 
   auto constexpr FmtStr1 = []() -> std::string {
      if constexpr (std::is_floating_point_v<ty1>) return "{:10.6Lf}"s;
      else                                         return "{:8Ld}"s;
      };

   auto constexpr FmtStr2 = []() -> std::string {
      if constexpr (std::is_floating_point_v<ty2>) return "{:10.6Lf}"s;
      else                                         return "{:8Ld}"s;
      };


   auto PointWithOther = [&out, FmtStr1, FmtStr2](auto const& p, bool boFirst) {
      if constexpr (knd1 != MyDistanceKind::without && knd2 != MyDistanceKind::without) {
         if constexpr (knd1 != knd2) {
            if (boFirst) myPrint(out, { " = ", FmtStr2(), "\n" }, p.convert_to<knd2>());
            else         myPrint(out, { " = ", FmtStr1(), "\n" }, p.convert_to<knd1>());
            }
         else out << "\n";
         }
      else out << "\n";
      };

   


   out << strText << "\n"
       << "-------------------------------------------------------\n";
   std::string fmt1, fmt2;
   if constexpr (std::is_floating_point_v<typename std::remove_const_t<std::remove_reference_t<decltype(p1)>>::value_type>) fmt1 = "{:.6Lf}"s;
   else fmt1 = "{:8Ld}"s;
   std::string fmtTxt { "{:<25s}"s };
   myPrintLn(out, { FmtStr1() }, p1);
   myPrint(out,   { fmtTxt, FmtStr1() }, "Pt1 =", p1);                                    PointWithOther(p1, true);
   myPrint(out,   { fmtTxt, FmtStr2() }, "Pt2 =", p2);                                    PointWithOther(p2, false);
   myPrint(out,   { fmtTxt, FmtStr1() }, "Pt1 + Pt2 =", p1 + p2);                         PointWithOther(p1 + p2, true);
   myPrint(out,   { fmtTxt, FmtStr1() }, "Pt1 - Pt2 =", p1 - p2);                         PointWithOther(p1 - p2, true);
   myPrint(out,   { fmtTxt, FmtStr1() }, "Pt1 * 5.0 =", p1 * 5.0);                        PointWithOther(p1 * 5.0, true);
   myPrint(out,   { fmtTxt, FmtStr1() }, "5.0 * Pt1 =", 5.0 * p1);                        PointWithOther(5.0 * p1, true);
   myPrint(out,   { fmtTxt, FmtStr1() }, "Pt1 * 5   =", p1 * 5);                          PointWithOther(p1 * 5, true);
   myPrint(out,   { fmtTxt, FmtStr1() }, "ScalarProd(Pt1, Pt2) =", p1.ScalarProduct(p2)); PointWithOther(p1.ScalarProduct(p2), true);
   myPrint(out,   { fmtTxt, FmtStr1() }, "Distance Pt1 - Pt2 =",   p1.Distance(p2));      PointWithOther(p1.Distance(p2), true);
   myPrint(out,   { fmtTxt, FmtStr1() }, "Distance Pt2 - Pt1=",    p2.Distance(p1));      PointWithOther(p2.Distance(p1), false);
   myPrint(out,   { fmtTxt, FmtStr1() }, "Magnitude Pt1 =",        p1.Magnitude());       PointWithOther(p1.Magnitude(), true);
   myPrint(out,   { fmtTxt, FmtStr1() }, "Magnitude Pt2 =",        p2.Magnitude());       PointWithOther(p2.Magnitude(), false);
   //*
   myPrintLn(out, { fmtTxt, "{:8.2Lf}" }, "Angle(Pt1, Pt2) deg =", p1.Angle(p2));
   myPrintLn(out, { fmtTxt, "{:8.6Lf}" }, "Angle(Pt1, Pt2) rad =", p1.Angle<MyAngleKind::radian>(p2));
   myPrintLn(out, { fmtTxt, "{:8.2Lf}" }, "Angle(Pt2, Pt1) =",     p2.Angle(p1));
   myPrintLn(out, { fmtTxt, "{:8.2Lf}", "  X-Axis = ", FmtStr1() }, "Angle(Pt1, x-Axis) deg =", p1.Angle(p1.X_Axis()), p1.X_Axis());
   myPrintLn(out, { fmtTxt, "{:8.2Lf}", "  Y-Axis = ", FmtStr1() }, "Angle(Pt1, y-Axis) deg =", p1.Angle(p1.Y_Axis()), p1.Y_Axis());
   if constexpr (SIZE1 == 3) {
      myPrintLn(out, { fmtTxt, "{:8.2Lf}", "  Z-Axis = ", FmtStr1() }, "Angle(Pt1, z-Axis) deg =", p1.Angle(p1.Z_Axis()), p1.Z_Axis());
      }
   myPrintLn(out, { fmtTxt, "{:8.2Lf}", "  X-Axis = ", FmtStr2() }, "Angle(Pt2, x-Axis) deg =", p2.Angle(p2.X_Axis()), p2.X_Axis());
   myPrintLn(out, { fmtTxt, "{:8.2Lf}", "  Y-Axis = ", FmtStr2() }, "Angle(Pt2, y-Axis) deg =", p2.Angle(p2.Y_Axis()), p2.Y_Axis());

   if constexpr (SIZE1 == 3) {
      myPrintLn(out, { fmtTxt, "{:8.2Lf}", "  Z-Axis = ", FmtStr2() }, "Angle(Pt2, z-Axis) deg =", p2.Angle(p2.Z_Axis()), p2.Z_Axis());
      }
   //*/
   myPrintLn(out, { fmtTxt, FmtStr1() }, "Center Pt1 - Pt2 =", p1.Center(p2));
   myPrintLn(out, { fmtTxt, FmtStr2() }, "Center Pt2 - Pt1 =", p2.Center(p1));
   myPrintLn(out, { fmtTxt, FmtStr1() }, "Unit vector Pt1 =",  p1.UnitVector());
   myPrintLn(out, { fmtTxt, FmtStr2() }, "Unit vector Pt2 =",  p2.UnitVector());

   if constexpr (SIZE1 == 3) {
      myPrintLn(out, { fmtTxt, FmtStr1() }, "CrossProd(Pt1, Pt2) =",   p1.CrossProduct(p2));
      myPrintLn(out, { fmtTxt, FmtStr1() }, "Magnitude of CrossP =",   p1.CrossProduct(p2).Magnitude());
      myPrintLn(out, { fmtTxt, FmtStr1() }, "Unit vector of CrossP =", p1.CrossProduct(p2).UnitVector());
      myPrintLn(out, { fmtTxt, FmtStr1() }, "Magnitude Unit of CP =",  p1.CrossProduct(p2).UnitVector().Magnitude());
      }

   if constexpr (!(knd1 == MyDistanceKind::without || oth_knd == MyDistanceKind::without)) {
      if constexpr (oth_knd != MyDistanceKind::without) 
         myPrintLn(out, { fmtTxt, FmtStr1(), " = ",  FmtStr1() }, "Pt1 X  =", p1.X(), p1.X<oth_knd>());
      else 
         myPrintLn(out, { fmtTxt, FmtStr1() }, "Pt1 X  =", p1.X());

      if constexpr (oth_knd != MyDistanceKind::without) 
         myPrintLn(out, { fmtTxt, FmtStr1(), " = ",  FmtStr1() }, "Pt1 Y  =", p1.Y(), p1.Y<oth_knd>());
      else 
         myPrintLn(out, { fmtTxt, FmtStr1() }, "Pt1 Y  =", p1.Y());
      if constexpr (SIZE1 == 3) {
         if constexpr (oth_knd != MyDistanceKind::without)
            myPrintLn(out, { fmtTxt, FmtStr1(), " = ",  FmtStr1() }, "Pt1 Z  =", p1.Z(), p1.Z<oth_knd>());
         else
            myPrintLn(out, { fmtTxt, FmtStr1() }, "Pt1 Z  =", p1.Z());
         }
      }
   myPrintLn(out, { "\n Test for points ", FmtStr1(), " and ", FmtStr2(), "... done.\n" }, p1, p2);
   }



void Test4Point(std::ostream& out) {
   out << "Method to test the MyPoint<> coordinates\n";
   // play with MyPoint
   // ------------------------------------------------------------------------------------------------------
   MyPoint<double, 3> test_without1(7.5, 2.1, 4.1), test_without2(4.3, 1.5, 1.0);
   out << test_without1.Null_Point() << "\n";
   out << -test_without1 << "\n";
   TestSequenze<MyDistanceKind::without>(out, "3d point without unit", test_without1, test_without2);

   // ------------------------------------------------------------------------------------------------------
   MyPoint<double, 3, MyDistanceKind::meter> test_meter1(3.5, 2.1, 1.0), test_meter2(4.3, 1.5, 2.1);
   TestSequenze< MyDistanceKind::yards>(out, "3 dimensional points with meter", test_meter1, test_meter2);

   MyPoint<double, 3, MyDistanceKind::yards> test_yard2 = { 40.5, 10.0, 3.0 };
   TestSequenze<MyDistanceKind::yards>(out, "operations with meters and yards", test_meter1, test_yard2);

   // ------------------------------------------------------------------------------------------------------
   MyPoint<int, 2> test1_2d(4, 3), test2_2d(5, 3);
   TestSequenze<MyDistanceKind::without>(out, "2 dimenional points without unit in int", test1_2d, test2_2d);
 
   MyPoint<double, 2, MyDistanceKind::yards> test_yard1_2d(40.5, 10.0), test_yard2_2d(10.0, 20.0);
   TestSequenze<MyDistanceKind::meter>(out, "2 dimenional points with yards", test_yard1_2d, test_yard2_2d);
   
   // ------------------------------------------------------------------------------------------------------
   out << "\nTests for different assignments\n";
   MyPoint<double, 3>                        test_copy_without = test_without1;
   MyPoint<double, 3, MyDistanceKind::meter> test_copy_meter_meter = test_meter1;
   MyPoint<double, 3, MyDistanceKind::yards> test_copy_meter_yards = test_meter1;
   out << "same type (without): " << test_copy_without << " = " << test_without1 << "\n"
       << "same type (meter)    " << test_copy_meter_meter << " = " << test_meter1 << "\n"
       << "meter to yard:       " << test_copy_meter_yards << " = " << test_meter1 << "\n";
   // the next line should produce an error because MyDistanceKind::without can't convert to a distance with unit
   //MyPoint<double, 3, MyDistanceKind::meter>   test_err_meter   = test_without1;
   //MyPoint<double, 3>                          test_err_without = test_meter1;

   out << "\nTests binary operations with different units and types\n";
   MyPoint<double, 3, MyDistanceKind::yards> test_yard_3d = { 23.0, 5.0, 0.0 };
   out << "Pkt_m_1 =           " << test_meter1 << "    = " << test_meter1.convert_to<MyDistanceKind::yards>() << "\n"
       << "Pkt_y_3d =          " << test_yard_3d << " = " << test_yard_3d.convert_to<MyDistanceKind::meter>() << "\n";
   out << "add meter + yard    " << test_meter1 + test_yard_3d << "\n"
       << "add yard + meter    " << test_yard_3d + test_meter1 << "\n";


   out << "\n\nspecial Tests for conversions and binary operations with different units and types\n";
   out << test_without1.X() << " as distance without unit combined with double values\n"
      << "5.0 + x = " << 5.0 + static_cast<double>(test_without1.X()) << "\n"
      << "x + 5.0 = " << test_without1.X() + 5.0 << "\n"
      << "x / 2 = " << test_without1.X() / 2 << "\n";

   }

} // end namespace
