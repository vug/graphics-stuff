#include "ResourceHandler.h"
#include "macros.h"

int main()
{
  PRINT_EXPR(int handle1 = ResourceHandler::createResource());
  PRINT_EXPR(int handle2 = ResourceHandler::createResource());
  PRINT_EXPR(ResourceHandler::useResource(handle1));
  EXCEPTION(ResourceHandler::useResource(5));
  EXCEPTION(ResourceHandler::deleteResource(5));
  PRINT_EXPR(ResourceHandler::useResource(handle2));
  PRINT_EXPR(ResourceHandler::deleteResource(handle1));
  EXCEPTION(ResourceHandler::useResource(handle1));
  EXCEPTION(ResourceHandler::deleteResource(handle1));
}