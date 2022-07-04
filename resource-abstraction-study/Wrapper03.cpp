#include "Wrapper03.h"

#include <iostream>
#include <string>

Wrapper::Wrapper()
    : handle(ResourceHandler::createResource()), state("<0-state>")
{
  printf("Constructing... (handle, state) (%d, %s)\n", handle, state.c_str());
}

Wrapper::Wrapper(std::string state)
    : handle(ResourceHandler::createResource()), state(state)
{
  printf("Constructing... (handle, state) (%d, %s)\n", handle, state.c_str());
}

Wrapper::~Wrapper()
{
  printf("Destructing... (handle, state) (%d, %s) -> (-2, %s(destructed))\n", handle, state.c_str(), state.c_str());
  if (handle > 0)
  {
    ResourceHandler::deleteResource(handle);
    handle = -2;
  }
  state += "(destructed)";
}

Wrapper::Wrapper(Wrapper &&other)
    : handle(std::move(other.handle)), state(std::move(other.state) + "(moved)")
{
  other.handle = -3;
  printf("Move constructing... (handle, state) dst: (%d, %s), src: (%d, %s)\n", handle, state.c_str(), other.handle, other.state.c_str());
}

Wrapper &Wrapper::operator=(Wrapper &&other)
{
  handle = std::move(other.handle);
  state = std::move(other.state) + "(moved)";
  other.handle = -3;
  printf("Move constructing... (handle, state) dst: (%d, %s), src: (%d, %s)\n", handle, state.c_str(), other.handle, other.state.c_str());
  return *this;
}

void Wrapper::use() const
{
  printf("Wrapper [%s] using Resource[%d]\n", state.c_str(), handle);
  ResourceHandler::useResource(handle);
}