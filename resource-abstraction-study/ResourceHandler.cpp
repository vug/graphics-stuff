#include "ResourceHandler.h"

#include <cassert>
#include <iostream>
#include <set>
#include <string>

int ResourceHandler::counter{};
std::unordered_set<int> ResourceHandler::availableResources{};

int ResourceHandler::createResource()
{
  auto [valueIterator, didInsert] = availableResources.insert(++counter);
  assert(didInsert); // since we increment counter at each call ids don't repeat and we should be able to insert
  std::cout << "Acquired Resource[" << *valueIterator << "] from ResourceHandler\n";
  return *valueIterator;
}

void ResourceHandler::useResource(int handle)
{
  if (!availableResources.contains(handle))
    throw std::exception{("[Exception] Attempting to use non-existing Resource[" + std::to_string(handle) + "]").c_str()};
  std::cout << "Used Resource[" << handle << "]\n";
}

void ResourceHandler::deleteResource(int handle)
{
  auto search = availableResources.find(handle);
  if (search == availableResources.end())
    throw std::exception{("[Exception] Attempting to delete non-existing Resource[" + std::to_string(handle) + "]").c_str()};
  availableResources.erase(search);
  std::cout << "Deleted Resource[" << handle << "] via ResourceHandler\n";
}
