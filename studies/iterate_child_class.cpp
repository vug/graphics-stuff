#include <iostream>
#include <vector>

class Base
{
public:
  int x;
  int y;
};

class Derived : public Base
{
public:
  int prop1;
};

int length2(const Base &b)
{
  return b.x * b.x + b.y * b.y;
}

// class Manager1
// {
// public:
//   Manager(std::vector<Base> &v) : objects(v) {}
//   std::vector<Base> &objects;
// };

template <typename T>
concept DerivedFromBase = std::is_base_of<Base, T>::value;

template <DerivedFromBase T>
// template <typename T>
class Manager
{
public:
  Manager(std::vector<T> &v) : objects(v) {}
  std::vector<T> &objects;

  int totalLength2() const
  {
    int tot = 0;
    for (const Base &b : objects)
      tot += length2(b);
    return tot;
  }
};

class NotDerived
{
public:
  NotDerived(int x) : prop1{x} {}
  int prop1;
};

int main()
{
  std::vector<Derived> vec = {{1, 2, 10}, {-1, 1, 11}, {-2, -3, 12}};

  for (size_t ix = 0; Base & b : vec)
  {
    printf("[%zu]: (%+d, %+d). length = %2d\n", ix, b.x, b.y, length2(b));
    ++ix;
  }

  Manager m{vec};
  printf("total length^2: %d\n", m.totalLength2());

  // std::vector<NotDerived> vec2 = {10, 11, 12, 13}; // fails here with concepts
  // Manager m2{vec2};
  // printf("total length^2: %d\n", m2.totalLength2()); // fails here without concept
}