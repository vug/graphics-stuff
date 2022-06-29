/**
 * A study of resource wrapping classes, and their creation and usage.
 *
 * build command: cl /EHsc factory_study.cpp /std:c++20 /Zi
 */
#include "ResourceHandler.h"
#include "macros.h"

#include <iostream>
#include <string>

class C
{
public:
  int handle = -1; // -1 means uninitialized, -2 means deleted, -3 means moved
  std::string name;

  C()
  {
    handle = ResourceHandler::createResource();
    name = "<no name>";
    std::cout << "Constructed C [" << handle << "]: " << name << "\n";
  }

  C(std::string name) : handle(ResourceHandler::createResource()), name(name)
  {
    std::cout << "Constructed C [" << handle << "]: " << name << "\n";
  }

  ~C()
  {
    const auto old_handle = handle;
    if (handle > 0)
    {
      ResourceHandler::deleteResource(handle);
      handle = -2;
    }
    name += "(destructed)";
    std::cout << "Destructed C [" << old_handle << "->" << handle << "]: " << name << "\n";
  }

  C(const C &other) : handle(other.handle), name(other.name + "(copy)")
  {
    std::cout << "Copy Constructed C [" << handle << "]: " << name << "\n";
  }

  C(C &&other) : handle(other.handle), name(other.name + "(moved)")
  {
    other.handle = -3;
    std::cout << "Move Constructed C [" << handle << "]: " << name << "\n";
  }

  C &operator=(const C &other)
  {
    const auto old_handle = handle;
    const auto old_name = name;
    handle = other.handle;
    name = other.name + "(copied)";
    std::cout << "Copied [" << old_handle << "->" << handle << "]: " << old_name << "->" << name << "\n";
    return *this;
  }

  C &operator=(C &&other)
  {
    const auto old_handle = handle;
    const auto old_name = name;
    handle = std::move(other.handle);
    name = std::move(other.name) + "(moved)";
    other.handle = -3;
    std::cout << "Moved [" << old_handle << "->" << handle << "]: " << old_name << "->" << name << "\n";
    return *this;
  }

private:
  static int counter;
};

int C::counter = 0;

C makeC_1()
{
  C c;
  c.handle = 1;
  c.name = "made with makeC_1";
  return c;
}

C makeC_2()
{
  return C{std::string("made with makeC_2")};
}

int main()
{
  // C c1;
  // C c2{"abidin"};
  // C c3{c1};
  // C c4{C{"osman"}};
  // C c5{std::move(C{"haydar"})};
  // c1 = c5;
  // C c6;
  // c6 = C{"riza"};
}