// testcout.cc
// very simple test of the C++ compiler and runtime

#include <iostream>     // cout

#ifdef TEST_COUT

int main()
{
  std::cout << "testcout: works\n";
  return 0;
}
#endif
