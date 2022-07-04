#include "Wrapper04.h"

#include "ResourceHandler.h"

#include <iostream>

Wrapper::Wrapper(const std::string &state)
    : handle(ResourceHandler::createResource()), state(state) {}

Wrapper::~Wrapper()
{
  ResourceHandler::deleteResource(handle);
}

std::shared_ptr<Wrapper> Wrapper::makeWrapper(const std::string &state)
{
  // can't use make_shared because it reference counting mechanism cannot access to private member Wrapper constructor
  // return std::make_shared<Wrapper>(state);
  return std::shared_ptr<Wrapper>(new Wrapper(state));
}

void Wrapper::use() const
{
  ResourceHandler::useResource(handle);
}