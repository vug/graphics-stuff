#pragma once

#include <unordered_set>

class ResourceHandler
{
public:
  static int createResource();
  static void useResource(int handle);
  static void deleteResource(int handle);

private:
  static int counter;
  static std::unordered_set<int> availableResources;
};