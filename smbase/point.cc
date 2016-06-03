// point.cc            see license.txt for copyright and terms of use
// code for point.h

#include "point.h"      // this module
#include "str.h"        // stringBuilder

std::stringstream& operator<< (std::stringstream &sb, point const &pt)
{
  return (std::stringstream&)(sb << "(" << pt.x << ", " << pt.y << ")");
}

std::stringstream& operator<< (std::stringstream &sb, fpoint const &pt)
{
  return (std::stringstream&)(sb << "(" << pt.x << ", " << pt.y << ")");
}
