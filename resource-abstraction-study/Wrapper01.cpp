#include "Wrapper01.h"

#include "ResourceHandler.h"

Wrapper::Wrapper(const std::string &state)
    : handle(ResourceHandler::createResource()), state(state) {}

Wrapper::~Wrapper()
{
  ResourceHandler::deleteResource(handle);
}

void Wrapper::use() const
{
  ResourceHandler::useResource(handle);
}