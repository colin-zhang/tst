// shared_ptr::get example
#include <iostream>
#include <memory>

#include "blake2.h"

static int* p = NULL;

void test()
{
  p = new int (10);
  std::shared_ptr<int> a (p);

  if (a.get()==p)
    std::cout << "a and p point to the same location\n";

  // three ways of accessing the same address:
  std::cout << p << "\n";
  std::cout << *a.get() << "\n";
  std::cout << *a << "\n";
  std::cout << a.use_count() << "\n";
  std::cout << *p << "\n";
}

int main () {
    
  test();
  std::cout << *p << "\n";
  return 0;
}

