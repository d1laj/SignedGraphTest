#include <iostream>

struct mystream {
  mystream() {}
};

struct myflush {
  myflush() {}
};

static mystream ms = mystream();

template <class T> mystream &operator<<(mystream &s, T truc) {
  std::cout << truc;
  std::cerr << truc;
  return s;
}

mystream &operator<<(mystream &s, myflush truc) {
  std::cout << std::flush;
  std::cerr << std::flush;
  return s;
}
