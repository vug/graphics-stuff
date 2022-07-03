#include "Wrapper02.h"

#include "ResourceHandler.h"

#include <iostream>

Wrapper::Wrapper(const std::string &state)
    : handle(ResourceHandler::createResource()), state(state) {}

Wrapper::~Wrapper()
{
  ResourceHandler::deleteResource(handle);
}

Wrapper::Wrapper(const Wrapper &other)
    : handle(other.handle), state(other.state)
{
  printf("Copy constructing... (handle, state) dst: (%d, %s), src: (%d, %s)\n", handle, state.c_str(), other.handle, other.state.c_str());
}

Wrapper &Wrapper::operator=(const Wrapper &other)
{
  printf("Copy assigning... (handle, state). dst: (%d, %s), src: (%d, %s)\n", handle, state.c_str(), other.handle, other.state.c_str());
  handle = other.handle;
  state = other.state;
  return *this;
}

Wrapper::Wrapper(Wrapper &&other)
    : handle(std::move(other.handle)), state(std::move(other.state))
{
  printf("Move constructing... (handle, state) dst: (%d, %s), src: (%d, %s)\n", handle, state.c_str(), other.handle, other.state.c_str());
}

Wrapper &Wrapper::operator=(Wrapper &&other)
{
  handle = std::move(other.handle);
  state = std::move(other.state);
  printf("Move assigning... (handle, state). dst: (%d, %s), src: (%d, %s)\n", handle, state.c_str(), other.handle, other.state.c_str());
  return *this;
}

void Wrapper::use() const
{
  ResourceHandler::useResource(handle);
}