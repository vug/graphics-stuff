#include "Wrapper.h"

#include <iostream>
#include <string>

Wrapper::Wrapper()
    : handle(ResourceHandler::createResource()), name("<no name>")
{
  std::cout << "Constructed [" << handle << "]: " << name << "\n";
}

Wrapper::Wrapper(std::string name)
    : handle(ResourceHandler::createResource()), name(name)
{
  std::cout << "Constructed [" << handle << "]: " << name << "\n";
}

Wrapper::~Wrapper()
{
  printf("Destructing [%s, %d]...\n", name.c_str(), handle);
  const auto old_handle = handle;
  if (handle > 0)
  {
    // Hard to catch exceptions happening in destructors while scope unwinding
    try
    {
      ResourceHandler::deleteResource(handle);
    }
    catch (const std::exception &e)
    {
      std::cerr << e.what() << '\n';
    }
    handle = -2;
  }
  name += "(destructed)";
  std::cout << "Destructed [" << old_handle << "->" << handle << "]: " << name << "\n";
}

// Wrapper::Wrapper(const Wrapper &other)
//     : handle(other.handle), name(other.name + "(copy)")
// {
//   std::cout << "Copy Constructed [" << handle << "]: " << name << "\n";
// }

Wrapper::Wrapper(Wrapper &&other)
    : handle(other.handle), name(other.name + "(moved)")
{
  other.handle = -3;
  std::cout << "Move Constructed [" << handle << "]: " << name << "\n";
}

// Wrapper &Wrapper::operator=(const Wrapper &other)
// {
//   const auto old_handle = handle;
//   const auto old_name = name;
//   handle = other.handle;
//   name = other.name + "(copied)";
//   std::cout << "Copy assigned [" << old_handle << "->" << handle << "]: " << old_name << "->" << name << "\n";
//   return *this;
// }

Wrapper &Wrapper::operator=(Wrapper &&other)
{
  const auto old_handle = handle;
  const auto old_name = name;
  handle = std::move(other.handle);
  name = std::move(other.name) + "(moved)";
  other.handle = -3;
  if (old_handle > 0)
    ResourceHandler::deleteResource(old_handle);
  std::cout << "Move assigned [" << old_handle << "->" << handle << "]: " << old_name << "->" << name << "\n";
  return *this;
}

void Wrapper::use() const
{
  std::cout << "Wrapper [" << name << "] ";
  ResourceHandler::useResource(handle);
}