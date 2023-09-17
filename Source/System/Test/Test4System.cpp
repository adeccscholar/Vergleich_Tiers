#include "Test4System.h"
#include <System/MyDistance.h>
#include <System/MyGeoLocation.h>

#include <iomanip>
#include <sstream>

namespace test_space {

// --------------------------------------------------------------
// method to call and test instances of the class MyDistance
// --------------------------------------------------------------
void Test4Distance(std::ostream& out) {
   using namespace my_distance_literals;
   out << "Method to test the MyDistance<kind, ty> class with different units\n";
   auto entfernung = 1000.0_meter;
   MyDistance<double, MyDistanceKind::kilometer> entfernung2{ entfernung };
   bool cc = (entfernung == entfernung2);
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

   out << "\nTest passed.\n\n";
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
   out << test.convert_to<MyAngleKind::degree>() << "\n";

   bool check = (test == cmp);
   auto calc = test + cmp;

   MyAngle<double, MyAngleKind::degree> tmp{ 0 };
   std::string w = "13";
   tmp.Angle(w);
   out << "Winkel als deg: " << tmp.Angle() << "\n";
   out << "Winkel als rad: " << tmp.Angle<MyAngleKind::radian>() << "\n";

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

template <MyDistanceKind oth_knd, MyDistanceKind knd1, my_param_distance ty1, int SIZE1, MyDistanceKind knd2, my_param_distance ty2, int SIZE2>
   requires (SIZE1 == SIZE2)
void TestSequenze(std::ostream& out, std::string const& strText, MyPoint<ty1, SIZE1, knd1> const& p1, MyPoint<ty2, SIZE2, knd2> const& p2) {
   auto PointWithOther = [](auto const& p, bool boFirst) -> std::string {
      std::ostringstream os;
      if constexpr (knd1 != MyDistanceKind::without && knd2 != MyDistanceKind::without) {
         if constexpr (knd1 != knd2) { 
            if(boFirst) os << " = " << p.convert_to<knd2>();
            else        os << " = " << p.convert_to<knd1>();
            }
         else os << "";
         }
      return os.str();
      };

   out << strText << "\n"
       << "-------------------------------------------------------\n";
   out << "Pt1 =                    " << p1 << PointWithOther(p1, true) << "\n"
      << "Pt2 =                    " << p2 << PointWithOther(p2, false) << "\n"
      << "Pt1 + Pt2 =              " << p1 + p2 << "\n"
      << "Pt1 - Pt2 =              " << p1 - p2 << "\n"
      << "Pt1 * 5.0 =              " << p1 * 5.0 << "\n"
      << "5.0 * Pt1 =              " << 5.0 * p1 << "\n"
      << "Pt1 * 5   =              " << p1 * 5 << "\n";
   //*
   out << "Angle(Pt1, Pt2) deg =    " << p1.Angle(p2) << "\n"
       << "Angle(Pt1, Pt2) rad =    " << p1.Angle<MyAngleKind::radian>(p2) << "\n"
       << "Angle(Pt2, Pt1) =        " << p2.Angle(p1) << "\n"
       << "Angle(Pt1, x-Axis) deg = " << p1.Angle(p1.X_Axis()) << "  " << p1.X_Axis() << "\n"
       << "Angle(Pt1, y-Axis) deg = " << p1.Angle(p1.Y_Axis()) << "  " << p1.Y_Axis() << "\n";
   if constexpr (SIZE1 == 3) {
      out << "Angle(Pt1, z-Axis) deg = " << p1.Angle(p1.Z_Axis()) << "  " << p1.Z_Axis() << "\n";
      }

   out << "Angle(Pt2, x-Axis) deg = " << p2.Angle(p2.X_Axis()) << "  " << p2.X_Axis() << "\n"
       << "Angle(Pt2, y-Axis) deg = " << p2.Angle(p2.Y_Axis()) << "  " << p2.Y_Axis() << "\n";
   if constexpr (SIZE1 == 3) {
      out << "Angle(Pt2, z-Axis) deg = " << p2.Angle(p2.Z_Axis()) << "  " << p2.Z_Axis() << "\n";
      }
   //*/
   out << "ScalarProd(Pt1, Pt2) =   " << p1.ScalarProduct(p2) << "\n"
       << "Distance Pt1 - Pt2   =   " << p1.Distance(p2) << PointWithOther(p1.Distance(p2), true)  << "\n"
       << "Distance Pt2 - Pt1   =   " << p2.Distance(p1) << PointWithOther(p2.Distance(p1), false) << "\n"
       << "Magnitude Pt1 =          " << p1.Magnitude()  << PointWithOther(p1.Magnitude(), true)   << "\n"
       << "Magnitude Pt2 =          " << p2.Magnitude()  << PointWithOther(p2.Magnitude(), false)  << "\n"
       << "Center Pt1 - Pt2 =       " << p1.Center(p2) << "\n"
       << "Center Pt2 - Pt1 =       " << p2.Center(p1) << "\n"
       << "Unit vector Pt1 =        " << p1.UnitVector() << "\n"
       << "Unit vector Pt2 =        " << p2.UnitVector() << "\n";

   if constexpr (!(knd1 == MyDistanceKind::without || oth_knd == MyDistanceKind::without)) {
      out << "Pt1 X  =         " << p1.X();
      if constexpr (oth_knd != MyDistanceKind::without) out << " = " << p1.X<oth_knd>() << "\n";
      else out << "\n";
      out << "Pt1 Y  =         " << p1.Y();
      if constexpr (oth_knd != MyDistanceKind::without) out << " = " << p1.Y<oth_knd>() << "\n";
      else out << "\n";
      if constexpr (SIZE1 == 3) {
         out << "Pt1 Z =          " << p1.Z();
         if constexpr (oth_knd != MyDistanceKind::without) out << " = " << p1.Z<oth_knd>() << "\n";
         else out << "\n";
         }
      }
   out << "\n";
   }

void Test4Point(std::ostream& out) {
   out << "Method to test the MyPoint<> coordinates\n";
   // play with MyPoint
   // ------------------------------------------------------------------------------------------------------
   MyPoint<double, 3> test_without1(7.5, 2.1, 4.1), test_without2(4.3, 1.5, 1.0);
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


   out << "\nspecial Tests for conversions and binary operations with different units and types\n";
   out << test_without1.X() << " as distance without unit combined with double values\n"
      << "5.0 + x = " << 5.0 + static_cast<double>(test_without1.X()) << "\n"
      << "x + 5.0 = " << test_without1.X() + 5.0 << "\n"
      << "x / 2 = " << test_without1.X() / 2 << "\n";

   // following lines are errors
   // out << 5.0 + static_cast<double>(test_meter1.X()) << "\n";
   // out << 5.0 + test_meter1.X() << "\n";

   /*
   out << test1 + test2 << "\n";
   out << "Point1 1:" << test1[0] << "\n";
   out << test2.Distance( { 7.8000, 3.6000, 3.1000 } ) << "\n";
   test1 *= 2;
   out << test1 << "\n";

   out << "\n";

   //test1 = { 3.5, 2.1, 1.0 };
   //test2 = { 4.3, 1.5, 2.1 };
   test1 = { 2.0, 2.0, 1.0 };
   test2 = { 1.0, 1.0, 0.0 };
   auto len1 = test1.Magnitude();
   auto len2 = test2.Magnitude();
   auto prod = test1.ScalarProduct(test2);
   auto pos  = test1.Angle(test2);
   out << "Point1: " << test1 << " len = " << len1 << "\n";
   out << "Point2: " << test2 << " len = " << len2 << "\n";
   out << "ScalarProduct = " << prod << "\n";
   out << "Angle: " << pos << "\n";

   auto center = test1.Center(test2);
   out << "Center: " << center << "\n";
   auto unit = test1.UnitVector();
   out << "Unit: " << unit << "\n"
       << "Length = " << unit.Magnitude() << "\n";


   prod = test1.ScalarProduct(test2);
   auto magn = test1.Magnitude();

   auto dist = test1.Distance(test2);
   out << "Distance = " << dist << "\n";

   //std::ranges::for_each(pos, [&out](auto const& val) { out << " - " << val << "\n";  });

   //auto ret = test1.CalculatePointAtDistanceAndAngles(dist, pos);
   //out << "ret = " << ret << "\n";
   */
}

} // end namespace
