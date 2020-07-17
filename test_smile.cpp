#include "catch.hpp"
#include "Shop.h"

/*
 * 
TEST_CASE("empty test")
{
   REQUIRE(1 == 1);
}
*
*/
/*
TEST_CASE("Convex Domain", "[convex]")
{
  //create a point
  Point aPoint = Point();
  double x = aPoint.x;
  double y = aPoint.y;
  int id = aPoint.id;
  
  //test construction of point
  SECTION("Point Construction")
  {  
     CHECK(x == 0.0);
     CHECK(y == 0.0);
     CHECK(id == -1);
  }
  
  //test init
  SECTION("init 0,1,10, and 1000 points")
  {  
     int count = 0;
     deque<Point> points;

     //check 0
     init(points, 0);
     CHECK(points.empty());
     //check 1
     init(points, 1);
     while (!points.empty())
     {
        points.pop_front();
        count += 1;
     }
     CHECK(count == 1);
     count = 0;
     
     //check 10
     init(points, 10);
     while (!points.empty())
     {
        points.pop_front();
        count += 1;
     }
     CHECK(count == 10);
     count = 0;
        
    //check 1000
     init(points, 1000);
     while (!points.empty())
     {
        points.pop_front();
        count += 1;
     }
     CHECK(count == 1000);
     count = 0;
     
  }

  //test divide
  SECTION("divide 0,1,11, and 1000 points")
  {  
     deque<Point> points, s1, s2;

     //check 0
     init(points, 0);
     divide(points, s1, s2);
     CHECK(s1.empty() == true);
     CHECK(s2.empty() == true);
     CHECK(points.size() == 0);
     while (!points.empty()) 
     { 
        points.pop_front();
     }
     
     
     //check 1
     init(points, 1);
     divide(points, s1, s2);
     CHECK(s1.empty() == true);
     CHECK(s2.size() == 1);
     CHECK(points.size() == 0);
     while (!points.empty()) 
     { 
        points.pop_front();
     }
     while (!s1.empty()) 
     { 
        s1.pop_front();
     }
     while (!s2.empty()) 
     { 
        s2.pop_front();
     }
     
     //check 11
     init(points, 11);
     divide(points, s1, s2);
     CHECK(s1.size() == 5);
     CHECK(s2.size() == 6);
     CHECK(points.size() == 0);
     while (!points.empty())
     {
        points.pop_front();
     }
     while (!s1.empty())
     {
        s1.pop_front();
     }
     while (!s2.empty())
     {
        s2.pop_front();
     }
        
    //check 1000
     init(points, 1000);
     divide(points, s1, s2);
     CHECK(s1.size() == 500);
     CHECK(s2.size() == 500);
     CHECK(points.size() == 0);
     while (!points.empty())
     {
        points.pop_front();
     }
     while (!s1.empty())
     {
        s1.pop_front();
     }
     while (!s2.empty())
     {
        s2.pop_front();
     }
  }
}
*/
