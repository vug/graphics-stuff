#include "Wrapper.h"

#include <iostream>
#include <string>

Wrapper::Wrapper()
{
  handle = ResourceHandler::createResource();
  name = "<no name>";
  std::cout << "Constructed [" << handle << "]: " << name << "\n";
}

Wrapper::Wrapper(std::string name) : handle(ResourceHandler::createResource()), name(name)
{
  std::cout << "Constructed [" << handle << "]: " << name << "\n";
}

Wrapper::~Wrapper()
{
  const auto old_handle = handle;
  if (handle > 0)
  {
    ResourceHandler::deleteResource(handle);
    handle = -2;
  }
  name += "(destructed)";
  std::cout << "Destructed [" << old_handle << "->" << handle << "]: " << name << "\n";
}

Wrapper::Wrapper(const Wrapper &other) : handle(other.handle), name(other.name + "(copy)")
{
  std::cout << "Copy Constructed [" << handle << "]: " << name << "\n";
}

Wrapper::Wrapper(Wrapper &&other) : handle(other.handle), name(other.name + "(moved)")
{
  other.handle = -3;
  std::cout << "Move Constructed [" << handle << "]: " << name << "\n";
}

Wrapper &Wrapper::operator=(const Wrapper &other)
{
  const auto old_handle = handle;
  const auto old_name = name;
  handle = other.handle;
  name = other.name + "(copied)";
  std::cout << "Copied [" << old_handle << "->" << handle << "]: " << old_name << "->" << name << "\n";
  return *this;
}

Wrapper &Wrapper::operator=(Wrapper &&other)
{
  const auto old_handle = handle;
  const auto old_name = name;
  handle = std::move(other.handle);
  name = std::move(other.name) + "(moved)";
  other.handle = -3;
  std::cout << "Moved [" << old_handle << "->" << handle << "]: " << old_name << "->" << name << "\n";
  return *this;
}

void Wrapper::use() const
{
  ResourceHandler::useResource(handle);
}