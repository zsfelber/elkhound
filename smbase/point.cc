// point.cc            see license.txt for copyright and terms of use
// code for point.h

#include "point.h"      // this module
#include "str.h"        // stringBuilder

str::stringstream& operator<< (str::stringstream &sb, point const &pt)
{
  return (str::stringstream&)(sb << "(" << pt.x << ", " << pt.y << ")");
}

str::stringstream& operator<< (str::stringstream &sb, fpoint const &pt)
{
  return (str::stringstream&)(sb << "(" << pt.x << ", " << pt.y << ")");
}
